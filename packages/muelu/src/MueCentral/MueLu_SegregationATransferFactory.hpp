/*
 * MueLu_SegregationATransferFactory.hpp
 *
 *  Created on: Oct 27, 2011
 *      Author: wiesner
 */

#ifndef MUELU_SEGREGATIONATRANSFERFACTORY_HPP
#define MUELU_SEGREGATIONATRANSFERFACTORY_HPP

#include "MueLu_ConfigDefs.hpp"

#include "MueLu_TwoLevelFactoryBase.hpp"
#include "MueLu_Level.hpp"
#include "MueLu_Exceptions.hpp"
//#include "MueLu_Utilities.hpp"

namespace MueLu {
/*!
  @class SegregationATransferFactory class.
  @brief special transfer factory for SegregationAFilterFactory
*/
  template <class Scalar = double, class LocalOrdinal = int, class GlobalOrdinal = LocalOrdinal, class Node = Kokkos::DefaultNode::DefaultNodeType, class LocalMatOps = typename Kokkos::DefaultKernels<void, LocalOrdinal, Node>::SparseOps>
  class SegregationATransferFactory : public TwoLevelFactoryBase {
#undef MUELU_SEGREGATIONATRANSFERFACTORY_SHORT
#include "MueLu_UseShortNames.hpp"

    typedef Xpetra::MapExtractor<Scalar, LocalOrdinal, GlobalOrdinal, Node> MapExtractorClass;
    typedef Xpetra::MapExtractorFactory<Scalar, LocalOrdinal, GlobalOrdinal, Node> MapExtractorFactoryClass;

  public:
    //@{ Constructors/Destructors.
    SegregationATransferFactory(RCP<FactoryBase> PtentFact = Teuchos::null)
    : PtentFact_(PtentFact) {}

    virtual ~SegregationATransferFactory() {}
    //@}

    //! Input
    //@{

    void DeclareInput(Level &fineLevel, Level &coarseLevel) const {
      coarseLevel.DeclareInput("P",PtentFact_.get());  // tentative prolongator, needed for finding corresponding coarse level gids
      fineLevel.DeclareInput("SegAMapExtractor", MueLu::NoFactory::get());
    }

    //@}

    //@{ Build methods.
    void Build(Level &fineLevel, Level &coarseLevel) const {
      TEUCHOS_TEST_FOR_EXCEPTION(!fineLevel.IsAvailable("SegAMapExtractor",MueLu::NoFactory::get()), Exceptions::RuntimeError, "MueLu::SegregationATransferFactory::Build(): SegAMapExtractor variable not available. Check if it has been generated and set in MueLu_SegregationAFilterFactory!");
      TEUCHOS_TEST_FOR_EXCEPTION(!coarseLevel.IsAvailable("P",PtentFact_.get()), Exceptions::RuntimeError, "MueLu::SegregationATransferFactory::Build(): P (generated by TentativePFactory not available.");

      // get map extractor and tentative variables from level classes
      RCP<const MapExtractorClass> fineMapExtractor = fineLevel.Get< RCP<const MapExtractorClass> >("SegAMapExtractor",MueLu::NoFactory::get());
      RCP<Operator> Ptent = coarseLevel.Get< RCP<Operator> >("P", PtentFact_.get());

      // prepare variables
      size_t numMaps = fineMapExtractor->NumMaps();
      std::vector<std::vector<GlobalOrdinal> > coarseGids(numMaps);

      // loop over local rows of Ptent
      for(size_t row=0; row<Ptent->getNodeNumRows(); row++)
      {
        // get global row id
        GlobalOrdinal grid = Ptent->getRowMap()->getGlobalElement(row); // global row id

        // check in which submap of mapextractor grid belongs to
        LocalOrdinal blockId = -Teuchos::ScalarTraits<LocalOrdinal>::one();
        for (size_t bb=0; bb<fineMapExtractor->NumMaps(); bb++) {
          const RCP<const Map> cmap = fineMapExtractor->getMap(bb);
          if (cmap->isNodeGlobalElement(grid)) {
            blockId = bb;
            break;
          }
        }
        TEUCHOS_TEST_FOR_EXCEPTION(blockId == -1, Exceptions::RuntimeError, "MueLu::SegregationATransferFactory::Build(): grid does not belong to a submap in mapextractor_");

        // extract data from Ptent
        size_t nnz = Ptent->getNumEntriesInLocalRow(row);
        TEUCHOS_TEST_FOR_EXCEPTION(nnz!=1, Exceptions::RuntimeError, "MueLu::SegregationATransferFactory::Build: number of nonzeros of Ptent != 1. Error.");
        Teuchos::ArrayView<const LocalOrdinal> indices;
        Teuchos::ArrayView<const Scalar> vals;
        Ptent->getLocalRowView(row, indices, vals);
        TEUCHOS_TEST_FOR_EXCEPTION(Teuchos::as<size_t>(indices.size()) != nnz, Exceptions::RuntimeError, "MueLu::SegregationATransferFactory::Build: number of nonzeros not equal to number of indices? Error.");

        // get global column id
        GlobalOrdinal gcid = Ptent->getColMap()->getGlobalElement(indices[0]); // LID -> GID (column)

        if(Ptent->getColMap()->isNodeLocalElement(indices[0])==true) {
          coarseGids[blockId].push_back(gcid);
        }
      }

      // build column maps
      std::vector<RCP<const Map> > colMaps(numMaps);
      for(size_t i=0; i<numMaps; i++) {
        std::vector<GlobalOrdinal> gidvector = coarseGids[i];
        std::sort(gidvector.begin(), gidvector.end());
        gidvector.erase(std::unique(gidvector.begin(), gidvector.end()), gidvector.end());
        TEUCHOS_TEST_FOR_EXCEPTION(gidvector.size()==0, Exceptions::RuntimeError, "MueLu::SegregationATransferFactory::Build: empty column map. Error.");
        if(gidvector.size()<15) GetOStream(Warnings0, 0) << "SegregationATransferFactory: submap " << i << "/" << numMaps << " for level " << coarseLevel.GetLevelID() << " has only " << gidvector.size() << " entries!" << std::endl;

        Teuchos::ArrayView<GlobalOrdinal> gidsForMap(&gidvector[0],gidvector.size());
        colMaps[i] = Xpetra::MapFactory<LocalOrdinal, GlobalOrdinal, Node>::Build(Ptent->getColMap()->lib(), gidsForMap.size(), gidsForMap, Ptent->getColMap()->getIndexBase(), Ptent->getColMap()->getComm());
        TEUCHOS_TEST_FOR_EXCEPTION(colMaps[i]==Teuchos::null, Exceptions::RuntimeError, "MueLu::SegregationATransferFactory::Build: error when building column map.");
        TEUCHOS_TEST_FOR_EXCEPTION((colMaps[i])->getGlobalNumElements()==0, Exceptions::RuntimeError, "MueLu::SegregationATransferFactory::Build: empty column map. Cannot be since gidvectors were not empty.");
        TEUCHOS_TEST_FOR_EXCEPTION((colMaps[i])->getGlobalNumElements()!=gidvector.size(), Exceptions::RuntimeError, "MueLu::SegregationATransferFactory::Build: size of map does not fit to size of gids.");
      }

      // build coarse level MapExtractor
      Teuchos::RCP<const MapExtractorClass> coarseMapExtractor = Xpetra::MapExtractorFactory<Scalar,LocalOrdinal,GlobalOrdinal,Node>::Build(Ptent->getDomainMap(),colMaps);

      // store map extractor in coarse level
      coarseLevel.Set("SegAMapExtractor", coarseMapExtractor, MueLu::NoFactory::get());
    }
    //@}

private:
  //! tentative P Factory (used to split maps)
  RCP<FactoryBase> PtentFact_;

}; //class SegregationATransferFactory

} //namespace MueLu

#define MUELU_SEGREGATIONATRANSFERFACTORY_SHORT

#endif // MUELU_SEGREGATIONATRANSFERFACTORY_HPP

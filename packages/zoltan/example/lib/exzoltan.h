/*
** $Id$
**
** Functions to support writing simple Zoltan examples 
*/

#include "zoltan.h"

#ifdef __cplusplus
extern "C" {
#endif

void exShowError(int val, char *s, int me);
int exGlobalSuccess(int rc, int nprocs, int me, int verbose);
void exPrintGlobalResult(char *s, int nprocs, int me,
  int begin, int import, int ex, int change);

/* build a mesh */
void exSetDivisions(int div);
int exInitializePoints(float **retPts, int **retIds, int rank, int size);
void exShowIds(int *ids, int n);
void exShowPoints(float *pts, int n, int *ids);

/* mesh query functions */
int exGetNumberOfAssignedObjects(void *userDefinedData, int *err);
int exGetObjectSize(void *userDefinedData, int *err);
void exGetObjectList(void *userDefinedData, int numGlobalIds, int numLids,
  ZOLTAN_ID_PTR gids, ZOLTAN_ID_PTR lids, int wgt_dim, float *obj_wgts,
  int *err);
void exGetObject(void *userDefinedData, int numGlobalIds, int numLids, 
  int numObjs, ZOLTAN_ID_PTR gids, ZOLTAN_ID_PTR lids, int numDim, 
  double *pts, int *err);

/* hypergraph options */

#define COMPLETE_ROWS   1
#define COMPLETE_COLS   2
#define BLOCKS          3

#define OMIT_EDGE_WEIGHTS      1
#define ALL_HAVE_EDGE_WEIGHTS  2
#define SOME_HAVE_EDGE_WEIGHTS 3
#define ONE_HAS_EDGE_WEIGHTS   4

void *set_initial_division(
  int initial_partitioning, /* COMPLETE_ROWS, COMPLETE_COLS or BLOCKS */
  int who_has_edge_weights, /* OMIT_EDGE_WEIGHTS, ALL_HAVE_EDGE_WEIGHTS, etc. */
  int edge_weights_overlap, /* 1 or 0 */
  int format);          /* ZOLTAN_COMPRESSED_ROWS or ZOLTAN_COMPRESSED_COLS */

/* hypergraph query functions */

int get_hg_size_and_format(void *data, 
  int *num_lists, int *num_pins, int *format);

int get_hg(void *data,  int num_gid_entries,
  int nrowcol, int npins, int format,
  ZOLTAN_ID_PTR rowcol_GID, int *rowcol_ptr, ZOLTAN_ID_PTR pin_GID);

int get_hg_edge_weight_size(void *data, int *num_edges);

int get_hg_edge_weights(void *data,  int num_gid_entries,
  int num_lid_entries, int nedges, int edge_weight_dim,
  ZOLTAN_ID_PTR edge_GID, ZOLTAN_ID_PTR edge_LID, float *edge_weight);
#ifdef __cplusplus
}
#endif






INCLUDE(Parse_Variable_Arguments)
INCLUDE(Global_Null_Set)
INCLUDE(Append_Global_Set)
INCLUDE(Print_Var)
INCLUDE(Remove_Global_Duplicates)


#
# Macro called at the very beginning of a Trilinos package's top-level
# CMakeLists.txt file
#

MACRO(TRILINOS_PACKAGE PACKAGE_NAME_IN)

  IF (Trilinos_VERBOSE_CONFIGURE)
    MESSAGE("\nTRILINOS_PACKAGE: ${PACKAGE_NAME_IN}")
  ENDIF()

  # A) Assert that the global and local package names are the same!

  IF (DEFINED PACKAGE_NAME_GLOBAL)
    IF (NOT ${PACKAGE_NAME_IN} STREQUAL ${PACKAGE_NAME_GLOBAL})
      MESSAGE(FATAL_ERROR "Error, the package-defined package name '${PACKAGE_NAME_IN}' is not the same as the package name defined at the global level '${PACKAGE_NAME_GLOBAL}'")
    ENDIF()
  ENDIF()

  #
  # B) Set up the CMake support for this Trilinos package and define some
  # top-level varaibles.
  #

  SET(PACKAGE_NAME ${PACKAGE_NAME_IN})
  MESSAGE(STATUS "Processing enabled package: ${PACKAGE_NAME}")

  # Write PACKAGE versions of common variables
  SET(PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
  SET(PACKAGE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")

  # Get the name of the directory this Trilinos package is in
  FILE(TO_CMAKE_PATH ${CMAKE_CURRENT_SOURCE_DIR} STANDARD_PACKAGE_SOURCE_DIR)
  STRING(REGEX REPLACE "/.+/(.+)" "\\1" PACKAGE_DIR_NAME "${STANDARD_PACKAGE_SOURCE_DIR}")

  #
  # C) Define package linkage varaibles
  #

  GLOBAL_NULL_SET(${PACKAGE_NAME}_INCLUDE_DIRS)
  GLOBAL_NULL_SET(${PACKAGE_NAME}_LIBRARY_DIRS)
  GLOBAL_NULL_SET(${PACKAGE_NAME}_LIBRARIES)
  GLOBAL_NULL_SET(${PACKAGE_NAME}_TEST_INCLUDE_DIRS)
  GLOBAL_NULL_SET(${PACKAGE_NAME}_TEST_LIBRARY_DIRS)
  GLOBAL_NULL_SET(${PACKAGE_NAME}_TEST_LIBRARIES)

  GLOBAL_NULL_SET(${PACKAGE_NAME}_LIB_TARGETS)
  GLOBAL_NULL_SET(${PACKAGE_NAME}_ALL_TARGETS)

  #
  # D) Define standard runtests targets for home-grown perl-based test harness
  #

  IF (Trilinos_ENABLE_NATIVE_TEST_HARNESS)
  
    ADD_CUSTOM_TARGET(
      ${PACKAGE_NAME}-runtests-serial
       ${PERL_EXECUTABLE} ${TRILINOS_HOME_DIR}/commonTools/test/utilities/runtests
      --trilinos-dir=${TRILINOS_HOME_DIR}
      --comm=serial
      --build-dir=${TRILINOS_BUILD_DIR}
      --category=${TRILINOS_TEST_CATEGORY}
      --output-dir=${TRILINOS_BUILD_DIR}/runtests-results
      --verbosity=1
      --packages=${PACKAGE_DIR_NAME}
      )

    IF (TPL_ENABLE_MPI)
    
      ADD_CUSTOM_TARGET(
        ${PACKAGE_NAME}-runtests-mpi
         ${PERL_EXECUTABLE} ${TRILINOS_HOME_DIR}/commonTools/test/utilities/runtests
        --trilinos-dir=${TRILINOS_HOME_DIR}
        --comm=mpi
        --mpi-go="${TRILINOS_MPI_GO}"
        --max-proc=${MPIEXEC_MAX_NUMPROCS}
        --build-dir=${TRILINOS_BUILD_DIR}
        --category=${TRILINOS_TEST_CATEGORY}
        --output-dir=${TRILINOS_BUILD_DIR}/runtests-results
        --verbosity=1
        --packages=${PACKAGE_DIR_NAME}
        )

    ENDIF()

  ENDIF()

ENDMACRO()


#
# Macro called to add a set of test directories for a package
#
# This macro only needs to be called from the top most CMakeList.txt file for
# which all subdirectories area all "tests".
#
# This macro can be called several times within a package and it will have the
# right effect.
#
# This macro defines hooks for inserting certain types of behavior in a
# uniform way.
#

MACRO(TRILINOS_PACKAGE_ADD_TEST_DIRECTORIES)

  IF(${PACKAGE_NAME}_ENABLE_TESTS)
    FOREACH(TEST_DIR ${ARGN})
      ADD_SUBDIRECTORY(${TEST_DIR})
    ENDFOREACH()
  ENDIF()

ENDMACRO()


#
# Macro called to add a set of performance test directories for a package
#
# This macro only needs to be called from the top most CMakeList.txt file for
# which all subdirectories area all "tests".
#
# This macro can be called several times within a package and it will have the
# right effect.
#
# This macro defines hooks for inserting certain types of behavior in a
# uniform way.
#

MACRO(TRILINOS_PACKAGE_ADD_PERFORMANCE_TEST_DIRECTORIES)

  IF(${PACKAGE_NAME}_ENABLE_PERFORMANCE_TESTS OR ${PACKAGE_NAME}_ENABLE_TESTS)
    FOREACH(TEST_DIR ${ARGN})
      ADD_SUBDIRECTORY(${TEST_DIR})
    ENDFOREACH()
  ENDIF()

ENDMACRO()


#
# Macro called to add a set of example directories for a package
#
# This macro only needs to be called from the top most CMakeList.txt file for
# which all subdirectories area all "examples".
#
# This macro can be called several times within a package and it will have the
# right effect.
#
# This macro defines hooks for inserting certain types of behavior in a
# uniform way.
#

MACRO(TRILINOS_PACKAGE_ADD_EXAMPLE_DIRECTORIES)

  IF(${PACKAGE_NAME}_ENABLE_EXAMPLES)
    FOREACH(EXAMPLE_DIR ${ARGN})
      ADD_SUBDIRECTORY(${EXAMPLE_DIR})
    ENDFOREACH()
  ENDIF()

ENDMACRO()


#
# Macro called at the very end of a Trilinos package's top-level
# CMakeLists.txt file
#

MACRO(TRILINOS_PACKAGE_POSTPROCESS)

  ADD_CUSTOM_TARGET(${PACKAGE_NAME}_libs DEPENDS ${${PACKAGE_NAME}_LIB_TARGETS})
  ADD_CUSTOM_TARGET(${PACKAGE_NAME}_all DEPENDS ${${PACKAGE_NAME}_ALL_TARGETS})

  IF (Trilinos_VERBOSE_CONFIGURE)
    MESSAGE("\nTRILINOS_PACKAGE_POSTPROCESS: ${PACKAGE_NAME}")
    PRINT_VAR(${PACKAGE_NAME}_INCLUDE_DIRS)
    PRINT_VAR(${PACKAGE_NAME}_LIBRARY_DIRS)
    PRINT_VAR(${PACKAGE_NAME}_LIBRARIES)
  ENDIF()

ENDMACRO()


#
# Private
#

MACRO(TRILINOS_PRIVATE_ADD_DEP_PACKAGES_INCLUDES_AND_LIBS PACKAGE_NAME TYPE DEP_LIST_TYPE)
  FOREACH(DEP_PKG ${${PACKAGE_NAME}_${DEP_LIST_TYPE}})
    TRILINOS_PRIVATE_ADD_DEP_PACKAGE_INCLUDES_AND_LIBS(${PACKAGE_NAME} "${TYPE}" ${DEP_PKG})
  ENDFOREACH()
ENDMACRO()


MACRO(TRILINOS_PRIVATE_ADD_DEP_PACKAGE_INCLUDES_AND_LIBS PACKAGE_NAME TYPE DEP_PKG)
  APPEND_GLOBAL_SET(${PACKAGE_NAME}_${TYPE}INCLUDE_DIRS ${${DEP_PKG}_INCLUDE_DIRS})
  APPEND_GLOBAL_SET(${PACKAGE_NAME}_${TYPE}LIBRARY_DIRS ${${DEP_PKG}_LIBRARY_DIRS})
  APPEND_GLOBAL_SET(${PACKAGE_NAME}_${TYPE}LIBRARIES ${${DEP_PKG}_LIBRARIES})
ENDMACRO()


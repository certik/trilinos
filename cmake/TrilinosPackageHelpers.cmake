
INCLUDE(Parse_Variable_Arguments)
INCLUDE(Global_Set)


#
# Macro called at the very beginning of a Trilinos package's top-level
# CMakeLists.txt file
#

MACRO(TRILINOS_PACKAGE PACKAGE_NAME_IN)

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

  SET(PACKAGE_NAME "${PACKAGE_NAME_IN}")
  MESSAGE(STATUS "Processing enabled Trilinos package: ${PACKAGE_NAME}")

  # Write PACKAGE versions of common variables
  SET(PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
  SET(PACKAGE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")

  # Get the name of the directory this Trilinos package is in
  FILE(TO_CMAKE_PATH ${CMAKE_CURRENT_SOURCE_DIR} STANDARD_PACKAGE_SOURCE_DIR)
  STRING(REGEX REPLACE "/.+/(.+)" "\\1" PACKAGE_DIR_NAME "${STANDARD_PACKAGE_SOURCE_DIR}")

  #
  # C) Define package linkage varaibles
  #

  GLOBAL_SET(${PACKAGE_NAME}_INCLUDE_DIRS "")
  GLOBAL_SET(${PACKAGE_NAME}_LIBRARY_DIRS "")
  GLOBAL_SET(${PACKAGE_NAME}_LIBRARIES "")

  #
  # D) Set up package dependancy-related variables
  #

  # ToDo: Implement!

  #
  # E) Define standard runtests targets for home-grown perl-based test harness
  #

  IF (Trilinos_ENABLE_NATIVE_TEST_HARNESS)
  
    ADD_CUSTOM_TARGET(
      runtests-serial-${PACKAGE_DIR_NAME}
       ${PERL_EXECUTABLE} ${TRILINOS_HOME_DIR}/commonTools/test/utilities/runtests
      --trilinos-dir=${TRILINOS_HOME_DIR}
      --comm=serial
      --build-dir=${TRILINOS_BUILD_DIR}
      --category=${TRILINOS_TEST_CATEGORY}
      --output-dir=${TRILINOS_BUILD_DIR}/runtests-results
      --verbosity=1
      --packages=${PACKAGE_DIR_NAME}
      )

    IF (Trilinos_ENABLE_MPI)
    
      ADD_CUSTOM_TARGET(
        runtests-mpi-${PACKAGE_DIR_NAME}
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

  # 2008/10/07: rabartl: ToDo: Put in hooks for defining the make target
  # 'tests'

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

  # 2008/10/07: rabartl: ToDo: Put in hooks for defining the make target
  # 'tests'

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

  # 2008/10/07: rabartl: ToDo: Put in hooks for defining the make target
  # 'examples'

ENDMACRO()


#
# Macro called at the very end of a Trilinos package's top-level
# CMakeLists.txt file
#

MACRO(TRILINOS_PACKAGE_POSTPROCESS)

  LIST(REMOVE_DUPLICATES ${PACKAGE_NAME}_INCLUDE_DIRS)
  LIST(REMOVE_DUPLICATES ${PACKAGE_NAME}_LIBRARY_DIRS)
  LIST(REMOVE_DUPLICATES ${PACKAGE_NAME}_LIBRARIES)

  IF (Trilinos_VERBOSE_CONFIGURE)
    MESSAGE("\nTRILINOS_PACKAGE_POSTPROCESS: ${PACKAGE_NAME}")
    PRINT_VAR(${PACKAGE_NAME}_INCLUDE_DIRS)
    PRINT_VAR(${PACKAGE_NAME}_LIBRARY_DIRS)
    PRINT_VAR( ${PACKAGE_NAME}_LIBRARIES)
  ENDIF()

ENDMACRO()

INCLUDE(PackageAddTestHelpers)


#
# Add a test or a set of tests for a single executable.
#
# PACKAGE_ADD_TEST(
#   <execName>
#   [NOEXEPREFIX]
#   [NOEXESUFFIX]
#   [NAME <testName> | NAME_POSTFIX <testNamePostfix>]
#   [DIRECTORY <directory>]
#   [ADD_DIR_TO_NAME]
#   [ARGS "<arg1> <arg2> ..." "<arg3> <arg4> ..." ...
#     | POSTFIX_AND_ARGS_0 <postfix> <arg1> <arg2> ...
#       POSTFIX_AND_ARGS_1 ... ]
#   [COMM [serial] [mpi]]
#   [NUM_MPI_PROCS <numProcs>]
#   [HOST <host1> <host2> ...]
#   [XHOST <host1> <host2> ...]
#   [HOSTTYPE <hosttype1> <hosttype2> ...]
#   [XHOSTTYPE <hosttype1> <hosttype2> ...]
#   [STANDARD_PASS_OUTPUT
#     | PASS_REGULAR_EXPRESSION "<regex1>;<regex2>;..." 
#     | FAIL_REGULAR_EXPRESSION "<regex1>;<regex2>;..."]
#   )
#  
# The arguments to the function are as followes:
#
#   <execName>
#
#     The mandatory root name of the executable that will be run to define the
#     test.  The full name of the executable is assumed to be
#     ${PACKAGE_NAME}_<execName>.exe and this executable is assumed to exist
#     in the current binary directory from where this function is called from
#     a CMakeLists.txt file.  This names is the default naming of executables
#     created by the function PACKAGE_ADD_EXECUTABLE(<execName> ...).
#     However, any arbitrary execuable program or script can be called by
#     setting NOEXEPREFIX and NOEXESUFFIX and you can also give <execName> as
#     an absolute path.
#
#   NOEXEPREFIX
#
#     If specified, then the prefix ${PACKAGE_NAME}_ is not assumed to be
#     appended to <execName>.
#
#   NOEXESUFFIX
#
#     If specified, then the postfix '.exe' is not assumed to be post-pended
#     to <execName>.
#
#   NAME <testName>
#
#     If specified, gives the root name of the test.  If not specified, then
#     <testName> is taked to be <execName>.  The actual test name will always
#     prefixed as ${PACKAGE_NAME}_<testName> and as added in the call to the
#     built-in CMake command ADD_TEST(...).  The main purpose of this argument
#     is to allow multiple tests to be defined for the same execurable.
#     Otherwise, you can not do this because CTest requires all test names to
#     be globally unique in a single project.
#
#   NAME_POSTFIX <testNamePostfix>
#
#     If specified, gives a postfix that will be added to the name of the
#     executable in order build the name of the test.  If NAME argument is
#     given it will be selected over the NAME_POSTFIX.
#
#   DIRECTORY <directory>
#
#     If specified, then the executable is assumed to be in the directory
#     given by relative <directory>.
# 
#   ADD_DIR_TO_NAME
#
#     If specified the directory name that the test resides in will be added into
#     the name of the test after any package name is added and before the given
#     name of the test. the directory will have the package's base directory
#     stripped off so only the unique part of the test directory will be used.
#     All directory seperators will be changed into underscores.
#
#   ARGS "<arg1> <arg2> ..." "<arg3> <arg4> ..." ...
#
#     If specified, then a set of arguments can be passed in quotes.  If
#     multiple groups of arguments are passed in different quoted clusters of
#     arguments than a different test will be added for each set of arguments.
#     In this way, many different tests can be added for a single executable
#     in a single call to this function.  Each of these separate tests will be
#     named ${PACKAGE_NAME}_<testName>_xy where xy = 00, 01, 02, and so on.
#
#   POSTFIX_AND_ARGS_<IDX> <postfix> <arg1> <arg2> ...
#
#     If specified, gives a sequence of sets of test postfix names and
#     arguments lists for defining different tests.  For example, a set of
#     three different tests with argument lists can be specified as:
#     
#       POSTIFX_AND_ARGS_0 postfix1 --arg1 --arg2="dummy"
#       POSTIFX_AND_ARGS_1 postfix2  --arg2="fly"
#       POSTIFX_AND_ARGS_3 postfix3  --arg2="bags"
#
#     This will create three different test cases with the postfix names
#     'postfix1', 'postfix2', and 'postfix3'.  The indexes must be consecutive
#     starting a 0 and going up to (currently) 19.  The main advantages of
#     using these arguments instead of just 'ARGS' are that you can give
#     meaningful name to each test case and you can specify multiple arguments
#     without having to quote them and you can allow long argument lists to
#     span multiple lines.
#
#   COMM [serial] [mpi]
#
#     If specified, selects if the test will be added in serial and/or MPI
#     mode.  If the COMM argument is missing, the test will be added in both
#     serial and MPI builds of the code.
#
#   NUM_MPI_PROCS <numProcs>
#
#     If specified, gives the number of processes that the test will be
#     defined to run on and can also result in the test being exluded all
#     together based on comparison to MPI_EXEC_MAX_NUMPROCS.  *** ToDo: Finish
#     specification of this arugment! ***
#
#   HOST <host1> <host2> ...
#
#     If specified, gives a list of hostnames where the test will be included.
#     The current hostname is determined by the built-in CMake command
#     SITE_NAME(...).  On Linux/Unix systems, this is typically the value
#     returned by 'uname -n'.
#
#   XHOST <host1> <host2> ...
#
#     If specified, gives a list of hostnames where the test will *not* be
#     included.  This check is performed after the check for the hostnames in
#     the 'HOST' list if it should exist.  Therefore, this list exclusion list
#     overrides the 'HOST' inclusion list.
#
#   HOSTTYPE <hosttype1> <hosttype2> ...
#
#     If specified, gives the names of the host system type (given by
#     ${CMAKE_HOST_SYSTEM_NAME}) to include the test.  Typical host system
#     type names include 'Linux', 'Darwain' etc.
#
#   XHOSTTYPE <name1> <name2> ...
#
#     If specified, gives the names of the host system type to *not* include
#     the test.  This check is performed after the check for the host system
#     names in the 'HOSTTYPE' list if it should exist.  Therefore, this list
#     exclusion list overrides the 'HOSTTYPE' inclusion list.
#
#   STANDARD_PASS_OUTPUT
#
#     If specified, then the standard test output "End Result: TEST PASSED" is
#     greped for to determine success.  This is needed for MPI tests on some
#     platforms since the return value is unreliable.
#
#   PASS_REGULAR_EXPRESSION "<regex1>;<regex2>;..." 
#
#     If specified, then a test will be assumed to pass only if one of the
#     regular expressions <regex1>, <regex2> etc. match the output.
#     Otherwise, the test will fail.
#
#   FAIL_REGULAR_EXPRESSION "<regex1>;<regex2>;..."
#
#     If specified, then a test will be assumed to fail if one of the regular
#     expressions <regex1>, <regex2> etc. match the output.  Otherwise, the
#     test will pass.
#

FUNCTION(PACKAGE_ADD_TEST EXE_NAME)
   
  #
  # A) Parse the input arguments
  #

  # Allow for a maximum of 20 (0 through 19) postfix and argument blocks
  SET(MAX_NUM_POSTFIX_AND_ARGS_IDX 19)

  SET(POSTFIX_AND_ARGS_LIST "")
  FOREACH( POSTFIX_AND_ARGS_IDX RANGE ${MAX_NUM_POSTFIX_AND_ARGS_IDX})
    LIST( APPEND POSTFIX_AND_ARGS_LIST POSTFIX_AND_ARGS_${POSTFIX_AND_ARGS_IDX} )
  ENDFOREACH()
  #PRINT_VAR(POSTFIX_AND_ARGS_LIST)

  PARSE_ARGUMENTS(
     #prefix
     PARSE
     #lists
     "DIRECTORY;KEYWORDS;COMM;NUM_MPI_PROCS;ARGS;${POSTFIX_AND_ARGS_LIST};NAME;NAME_POSTFIX;HOST;XHOST;HOSTTYPE;XHOSTTYPE;PASS_REGULAR_EXPRESSION;FAIL_REGULAR_EXPRESSION"
     #options
     "NOEXEPREFIX;NOEXESUFFIX;STANDARD_PASS_OUTPUT;ADD_DIR_TO_NAME"
     ${ARGN}
     )

  IF (PARSE_ARGS)
    LIST(LENGTH PARSE_ARGS NUM_PARSE_ARGS)
  ELSEIF (PARSE_POSTFIX_AND_ARGS_0)
    # We will use this list instead
  ELSE()
    # Niether 'ARGS' nor 'POSTFIX_AND_ARGS' was selected so just assume one
    # empty arg
    SET(PARSE_ARGS " ")
    SET(NUM_PARSE_ARGS 1)
  ENDIF()

  IF (${PROJECT_NAME}_VERBOSE_CONFIGURE)
    MESSAGE("")
    MESSAGE("PACKAGE_ADD_TEST: EXE_NAME = ${EXE_NAME}")
  ENDIF()
  
  #
  # B) Do not add any tests if host/hostname or xhost/xhostname says not to
  #

  IF (NOT PARSE_HOST)
    SET (PARSE_HOST ${${PROJECT_NAME}_HOSTNAME})
  ENDIF()  
  LIST (FIND PARSE_HOST ${${PROJECT_NAME}_HOSTNAME} INDEX_OF_HOSTNAME_IN_HOST_LIST)                 
  IF (${INDEX_OF_HOSTNAME_IN_HOST_LIST} EQUAL -1)
    RETURN()
  ENDIF()
  
  IF (NOT PARSE_XHOST)
    SET (PARSE_XHOST NONE)
  ENDIF()    
  LIST (FIND PARSE_XHOST ${${PROJECT_NAME}_HOSTNAME} INDEX_OF_HOSTNAME_IN_XHOST_LIST)           
  IF (NOT ${INDEX_OF_HOSTNAME_IN_XHOST_LIST} EQUAL -1)
    RETURN()
  ENDIF()

  IF (NOT PARSE_HOSTTYPE)
    SET(PARSE_HOSTTYPE ${CMAKE_HOST_SYSTEM_NAME})
  ENDIF()
  LIST (FIND PARSE_HOSTTYPE ${CMAKE_HOST_SYSTEM_NAME} INDEX_OF_HOSTSYSTEMNAME_IN_HOSTTYPE_LIST)
  IF (${INDEX_OF_HOSTSYSTEMNAME_IN_HOSTTYPE_LIST} EQUAL -1)
    RETURN()
  ENDIF()

  IF (NOT PARSE_XHOSTTYPE)
    SET(PARSE_XHOSTTYPE NONE)
  ENDIF()
  LIST (FIND PARSE_XHOSTTYPE ${CMAKE_HOST_SYSTEM_NAME} INDEX_OF_HOSTSYSTEMNAME_IN_XHOSTTYPE_LIST)
  IF (NOT ${INDEX_OF_HOSTSYSTEMNAME_IN_XHOSTTYPE_LIST} EQUAL -1)
    RETURN()
  ENDIF()

  #
  # C) Set the name and path of the binary that will be run
  #

  PACKAGE_ADD_TEST_GET_EXE_BINARY_NAME( "${EXE_NAME}"
    ${PARSE_NOEXEPREFIX} ${PARSE_NOEXESUFFIX} ${PARSE_ADD_DIR_TO_NAME} EXE_BINARY_NAME )
  
  #If requested create a modifier for the name that will be inserted between the package name 
  #and the given name or exe_name for the test
  SET(DIRECTORY_NAME "")
  IF(PARSE_ADD_DIR_TO_NAME)
    PACKAGE_CREATE_NAME_FROM_CURRENT_SOURCE_DIRECTORY(DIRECTORY_NAME)
    SET(DIRECTORY_NAME "_${DIRECTORY_NAME}")
  ENDIF()

  #MESSAGE("PACKAGE_ADD_TEST: ${EXE_NAME}: EXE_BINARY_NAME = ${EXE_BINARY_NAME}")
  
  IF (PARSE_NAME)
    SET(TEST_NAME "${PACKAGE_NAME}${DIRECTORY_NAME}_${PARSE_NAME}")
  ELSEIF (PARSE_NAME_POSTFIX)
    SET(TEST_NAME "${PACKAGE_NAME}${DIRECTORY_NAME}_${EXE_NAME}_${PARSE_NAME_POSTFIX}")  
  ELSE()
    SET(TEST_NAME "${PACKAGE_NAME}${DIRECTORY_NAME}_${EXE_NAME}")  
  ENDIF()
  
  IF(PARSE_DIRECTORY)
    SET(EXECUTABLE_PATH "${PARSE_DIRECTORY}/${EXE_BINARY_NAME}")
  ELSE()
    SET(EXECUTABLE_PATH "${EXE_BINARY_NAME}")
  ENDIF()

  IF (NOT IS_ABSOLUTE ${EXECUTABLE_PATH})
    SET(EXECUTABLE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${EXE_BINARY_NAME}")
  ENDIF()

  #MESSAGE("PACKAGE_ADD_TEST: ${EXE_NAME}: EXECUTABLE_PATH = ${EXECUTABLE_PATH}")

  #
  # D) Append keywords to the name of the test
  #
  
  IF(PARSE_KEYWORDS)
    FOREACH(KEYWORD ${PARSE_KEYWORDS})
      SET(TEST_NAME ${TEST_NAME}_${KEYWORD})
    ENDFOREACH()
  ENDIF()

  #
  # E) Determine if we will add the serial or MPI tests based on input COMM
  # and TPL_ENABLE_MPI
  #

  PACKAGE_PROCESS_COMM_ARGS(ADD_SERIAL_TEST  ADD_MPI_TEST  ${PARSE_COMM})

  #
  # F) Get the MPI options
  #
    
  PACKAGE_ADD_TEST_GET_NUM_PROCS_USED("${PARSE_NUM_MPI_PROCS}" NUM_PROCS_USED)
  IF (NUM_PROCS_USED LESS 0)
    SET(ADD_MPI_TEST FALSE)
  ENDIF()

  #
  # G) Add the tests
  #

  IF (NOT ADD_SERIAL_TEST AND NOT ADD_MPI_TEST)
    RETURN()
  ENDIF()

  IF (TPL_ENABLE_MPI)
    SET(TEST_NAME "${TEST_NAME}_MPI_${NUM_PROCS_USED}")
  ENDIF()

  IF (PARSE_ARGS)

    # G.1) Add tests with simple lists of arguments
  
    SET(COUNTER 0)
  
    FOREACH(PARSE_ARG ${PARSE_ARGS})
  
      IF(${NUM_PARSE_ARGS} EQUAL 1)
        SET(TEST_NAME_INSTANCE "${TEST_NAME}")
      ELSE()
        SET(TEST_NAME_INSTANCE "${TEST_NAME}_${COUNTER}")
      ENDIF()
      IF(${PROJECT_NAME}_VERBOSE_CONFIGURE)
        MESSAGE(STATUS "TEST_NAME = ${TEST_NAME_INSTANCE}")
      ENDIF()
  
      CONVERT_CMND_ARG_STRING_TO_ADD_TEST_ARG_ARRAY(${PARSE_ARG} INARGS)
      IF (${PROJECT_NAME}_VERBOSE_CONFIGURE)
        PRINT_VAR(INARGS)
      ENDIF()
  
      PACAKGE_ADD_TEST_GET_TEST_CMND_ARRAY( CMND_ARRAY
        "${EXECUTABLE_PATH}"  "${NUM_PROCS_USED}"  ${INARGS} )
  
      PACKAGE_ADD_TEST_ADD_TEST( ${TEST_NAME_INSTANCE} ${CMND_ARRAY} )
      
      PACKAGE_PRIVATE_ADD_TEST_POST_PROCESS_ADDED_TEST(${TEST_NAME_INSTANCE})
  
      MATH(EXPR COUNTER ${COUNTER}+1 )
  
    ENDFOREACH()

  ELSEIF (PARSE_POSTFIX_AND_ARGS_0)

    # G.2) Add tests with different postfixes for each set of arguments

    FOREACH( POSTFIX_AND_ARGS_IDX RANGE ${MAX_NUM_POSTFIX_AND_ARGS_IDX})

      IF(${PROJECT_NAME}_VERBOSE_CONFIGURE)
        PRINT_VAR(PARSE_POSTFIX_AND_ARGS_${POSTFIX_AND_ARGS_IDX})
      ENDIF()

      IF (NOT PARSE_POSTFIX_AND_ARGS_${POSTFIX_AND_ARGS_IDX})
        BREAK()
      ENDIF()

      SET( POSTFIX_AND_ARGS ${PARSE_POSTFIX_AND_ARGS_${POSTFIX_AND_ARGS_IDX}} )

      LIST( GET  POSTFIX_AND_ARGS  0  POSTFIX )
      LIST( REMOVE_AT  POSTFIX_AND_ARGS  0 ) # Just jut args!

      SET(TEST_NAME_INSTANCE "${TEST_NAME}_${POSTFIX}")
  
      PACAKGE_ADD_TEST_GET_TEST_CMND_ARRAY( CMND_ARRAY
        "${EXECUTABLE_PATH}"  "${NUM_PROCS_USED}"  ${POSTFIX_AND_ARGS} )
  
      PACKAGE_ADD_TEST_ADD_TEST( ${TEST_NAME_INSTANCE} ${CMND_ARRAY} )
      
      PACKAGE_PRIVATE_ADD_TEST_POST_PROCESS_ADDED_TEST(${TEST_NAME_INSTANCE})


    ENDFOREACH()

  ENDIF()
  
ENDFUNCTION()

MACRO(EPETRA_ADD_TEST TEST_NAME)

  SET(TEST_EXEC_NAME ${TEST_NAME}_test)

  SET(KEYWORD )
  SET(TEST_SOURCES )
  SET(TEST_ARGS )
  SET(TEST_INSTALL 0)

  FOREACH(ARGUMENT ${ARGN})
    IF(ARGUMENT STREQUAL "SOURCES")
      SET(KEYWORD ${ARGUMENT})
    ELSEIF(ARGUMENT STREQUAL "ARGS")
      SET(KEYWORD ${ARGUMENT})
    ELSEIF(ARGUMENT STREQUAL "INSTALL")
      SET(TEST_INSTALL 1)
    ELSE(ARGUMENT STREQUAL "SOURCES")
      IF(KEYWORD STREQUAL "SOURCES")
        SET(TEST_SOURCES ${TEST_SOURCES} ${TEST_NAME}/${ARGUMENT})
      ELSEIF(KEYWORD STREQUAL "ARGS")
        SET(TEST_ARGS ${TEST_ARGS} ${ARGUMENT})
      ELSE(KEYWORD STREQUAL "SOURCES")
      ENDIF(KEYWORD STREQUAL "SOURCES")
    ENDIF(ARGUMENT STREQUAL "SOURCES")
  ENDFOREACH(ARGUMENT)

  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME})
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/${TEST_NAME})
  INCLUDE_DIRECTORIES(${epetra_BINARY_DIR}/src)
  INCLUDE_DIRECTORIES(${epetra_SOURCE_DIR}/src)

  ADD_DEFINITIONS(-DHAVE_CONFIG_H)

  ADD_EXECUTABLE(${TEST_EXEC_NAME} ${TEST_SOURCES})
  TARGET_LINK_LIBRARIES(${TEST_EXEC_NAME} epetra ${LAPACK_LIBRARY} ${BLAS_LIBRARY})

  IF(TEST_INSTALL)
    INSTALL(TARGETS ${TEST_EXEC_NAME} RUNTIME DESTINATION bin)
  ENDIF(TEST_INSTALL)

  IF(ENABLE_MPI)
    INCLUDE_DIRECTORIES(${MPI_INCLUDE_PATH})
    TARGET_LINK_LIBRARIES(${TEST_EXEC_NAME} ${MPI_LIBRARY})
  ENDIF(ENABLE_MPI)

  ADD_TEST(${TEST_NAME} ${TEST_EXEC_NAME} ${TEST_ARGS})

  IF(EPETRA_ENABLE_MPI)
    ADD_TEST(${TEST_NAME}_MPI ${MPI_EXECUTABLE} ${MPI_EXECUTABLE_FLAGS} ${TEST_EXEC_NAME} ${TEST_ARGS})
  ENDIF(EPETRA_ENABLE_MPI)

ENDMACRO(EPETRA_ADD_TEST) 


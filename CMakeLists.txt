CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
PROJECT (Parsim CXX)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

FIND_PACKAGE(Boost 1.46)

IF (Boost_FOUND)
ELSE()

  INCLUDE (ExternalProject)
  SET (BOOST_URL "http://sourceforge.net/projects/boost/files/boost/1.49.0/boost_1_49_0.tar.gz/download")
  ExternalProject_Add(
    ExternalBoost
    URL ${BOOST_URL}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    PREFIX ${PROJECT_SOURCE_DIR}/_deps
  )
  SET (Boost_INCLUDE_DIRS _deps/src/ExternalBoost)
  MESSAGE (WARNING "Boost headers will be downloaded from ${BOOST_URL}")  
ENDIF()


INCLUDE_DIRECTORIES (include ${Boost_INCLUDE_DIRS})
SET (SRC_TEST tests/test_parsers.cc)
ADD_EXECUTABLE(parsim_tests ${SRC_TEST})




ENABLE_TESTING()
ADD_TEST(parsim_tests parsim_tests)

INSTALL(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX})
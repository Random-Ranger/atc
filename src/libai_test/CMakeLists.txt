cmake_minimum_required(VERSION 3.9)
project(libai_test CXX)

include(CTest)
include(GoogleTest)
enable_testing()

add_executable(libai_test 
    maneuverFactoryTest.cpp
    workflowTest.cpp
    mutexTestCase.hpp
    mutexLongRunningTestCase.hpp
    createAirportEFGH.cpp
    simpleRunwayMutexTest.cpp
    runwayMutexSequenceTest1.cpp
    runwayMutexSequenceTest2.cpp
    runwayMutexSequenceTest3.cpp
    runwayMutexSequenceTest4.cpp
    runwayMutexSequenceTest5.cpp
)

set_property(TARGET libai_test PROPERTY CXX_STANDARD 14)
target_include_directories(libai_test PUBLIC ../libworld ../libai ../libworld_test)
target_link_libraries(libai_test libai libworld GTest::GTest GTest::Main)

if (ATCBUILD_CAN_RUN_TESTS)
    gtest_discover_tests(
        libai_test
        PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

cmake_minimum_required(VERSION 3.16)

enable_testing()

include(FetchContent)
FetchContent_Declare(googletest
		GIT_REPOSITORY https://github.com/google/googletest.git
		GIT_TAG "v1.17.0"
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(MGN_TESTS_SOURCES
	Sources/TestCoreHeapArray.cpp
		Sources/TestCoreSparseSet.cpp
		Sources/GlobalUsefullTests.hpp
		Sources/TestRTTI.cpp
		Sources/TestScene.cpp
		Sources/TestCoreRawSparseSet.cpp
)

add_executable(MGN_Tests ${MGN_TESTS_SOURCES})

target_link_libraries( MGN_Tests PUBLIC
		Imagine::Core
		GTest::gtest_main
)

target_include_directories(MGN_Tests PUBLIC Sources)
target_precompile_headers(MGN_Tests REUSE_FROM Core)

include(GoogleTest)
gtest_discover_tests(MGN_Tests)
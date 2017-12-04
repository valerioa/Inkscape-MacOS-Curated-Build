# -----------------------------------------------------------------------------
#
# -----------------------------------------------------------------------------
if(NOT GTEST_FOUND) # not required for gtest 1.8.0 and later
    add_subdirectory(${GMOCK_DIR} ${CMAKE_BINARY_DIR}/gmock)

    include_directories(SYSTEM ${GMOCK_DIR}/gtest/include
                               ${GMOCK_DIR}/include
                               ${CMAKE_SOURCE_DIR}/gtest/gtest/include)
endif()

add_executable(tests
	unittest.cpp
	doc-per-case-test.cpp
	src/attributes-test.cpp
	src/color-profile-test.cpp
	src/dir-util-test.cpp
)

target_link_libraries(tests
	gmock_main
	inkscape_base)

add_test(BaseTest ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests)

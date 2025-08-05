aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} cpps)
add_executable(agg2_common_test ${cpps})
target_link_libraries(agg2_common_test a4_agg_web2 test_main)
add_test (NAME test_agg2_common COMMAND ${CMAKE_CURRENT_BINARY_DIR}/agg2_common_test WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

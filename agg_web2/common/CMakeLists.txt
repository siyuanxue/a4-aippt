add_subdirectory(qrs)
add_subdirectory(data_wrapper)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} cpps)
add_library(aggweb2_common OBJECT ${cpps})

#add_subdirectory(test)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} cpps)
add_library(aggweb2_loader OBJECT ${cpps})

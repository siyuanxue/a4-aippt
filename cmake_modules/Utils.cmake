macro(INSTALL_LIB target component)
    install(TARGETS ${target}
        LIBRARY DESTINATION lib64
        COMPONENT ${component})
endmacro()

macro(INSTALL_BIN target component)
    install(TARGETS ${target}
        RUNTIME DESTINATION bin
        COMPONENT ${component})
endmacro()

macro(INSTALL_ARCHIVE target component)
    install(TARGETS ${target}
        ARCHIVE DESTINATION lib64
        COMPONENT ${component})
endmacro()

macro(INSTALL_HEADER_FILES src_dir dst_dir component)
    if(EXISTS "${src_dir}/") 
        file(GLOB h_files ${src_dir}/*.h ${src_dir}/*.hpp)
        install(FILES ${h_files} 
            DESTINATION ${dst_dir}
            COMPONENT ${component})
    else()
        install(FILES ${src_dir} 
            DESTINATION ${dst_dir}
            COMPONENT ${component})
    endif()
endmacro()

macro(CHECK_PROTOC)
    find_program(PROTOBUF_BIN protoc PATHS ${PROJECT_SOURCE_DIR}/_external/usr/bin/ ${PROJECT_SOURCE_DIR}/_external/usr/local/bin/)
    if(PROTOBUF_BIN-NOTFOUND)
        message(FATAL_ERROR "protoc not found.")
    else()
        message(STATUS "protoc found: ${PROTOBUF_BIN}")
    endif()
endmacro()

macro(COMPILE_PROTO dir dst_dir PROTO_SRCS PROTO_HEADERS)
    set(srcs "")
    set(headers  "")
    FILE(GLOB proto_files ${dir}/*.proto)  
    FOREACH(proto_file ${proto_files})
        get_filename_component(bare_file_name ${proto_file} NAME_WE)
        LIST(APPEND srcs "${dst_dir}/${bare_file_name}.pb.cc")
        LIST(APPEND headers "${dst_dir}/${bare_file_name}.pb.h")
        ADD_CUSTOM_COMMAND(
            OUTPUT ${dst_dir}/${bare_file_name}.pb.cc ${dst_dir}/${bare_file_name}.pb.h
            COMMAND ${PROTOBUF_BIN} --proto_path=${dir}
                           --cpp_out=${dst_dir} ${proto_file} 
            DEPENDS ${proto_file}
        )
    ENDFOREACH(proto_file)
    set(PROTO_SRCS ${srcs})
    set(PROTO_HEADERS ${headers})
endmacro()

macro(MACRO_ENSURE_OUT_OF_SOURCE_BUILD MSG)
    STRING(COMPARE EQUAL "${CMAKE_SOURCE_DIR}"
"${CMAKE_BINARY_DIR}" insource)
    GET_FILENAME_COMPONENT(PARENTDIR ${CMAKE_SOURCE_DIR} PATH)
    STRING(COMPARE EQUAL "${CMAKE_SOURCE_DIR}"
"${PARENTDIR}" insourcesubdir)
    IF(insource OR insourcesubdir)
        MESSAGE(FATAL_ERROR "${MSG}")
    ENDIF(insource OR insourcesubdir)
endmacro(MACRO_ENSURE_OUT_OF_SOURCE_BUILD)

macro(GIT_REF_REV DIR)
    find_program(GIT_BIN "git")
    find_program(RPM_BIN "rpm")
    if(GIT_BIN-NOTFOUND)
        message(FATAL_ERROR "git not found.")
    endif(GIT_BIN-NOTFOUND)
    if(RPM_BIN-NOTFOUND)
        message(FATAL_ERROR "rpm not found.")
    endif(RPM_BIN-NOTFOUND)
    execute_process(
        COMMAND bash -c "${GIT_BIN} symbolic-ref -q --short HEAD||${GIT_BIN} describe --tags --exact-match"
        WORKING_DIRECTORY "${DIR}"
        OUTPUT_VARIABLE GIT_REF
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    execute_process(
        COMMAND bash -c "${GIT_BIN} rev-parse --short HEAD"
        WORKING_DIRECTORY "${DIR}"
        OUTPUT_VARIABLE GIT_REV
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    execute_process(
        COMMAND bash -c "git log --pretty=format:'(%h) (%ad) (%s) %d (%ae)' -1"
        WORKING_DIRECTORY "${DIR}"
        OUTPUT_VARIABLE GIT_LOG
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    execute_process(
        COMMAND bash -c "date -d @$(${GIT_BIN} log -1 --format='%at') +%Y-%m-%d_%H:%M:%S"
        WORKING_DIRECTORY "${DIR}"
        OUTPUT_VARIABLE GIT_DATE
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    execute_process(
        COMMAND bash -c "date -d now +%Y-%m-%d_%H:%M:%S"
        WORKING_DIRECTORY "${DIR}"
        OUTPUT_VARIABLE BUILD_TIME
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    execute_process(
        COMMAND bash -c "whoami"
        WORKING_DIRECTORY "${DIR}"
        OUTPUT_VARIABLE BUILD_USER
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    execute_process(
        COMMAND bash -c "${RPM_BIN} -qa --root=${DIR}/_external | tr '\n' ' '"
        WORKING_DIRECTORY "${DIR}"
        OUTPUT_VARIABLE BUILDENV_RPM
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    cmake_host_system_information(RESULT BUILD_HOST QUERY HOSTNAME)
endmacro(GIT_REF_REV)

macro(BUILD_TEST_WITH_DEPENDS test_name)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} cpps)
    add_executable(${test_name} ${cpps})
    target_link_libraries(${test_name} ${ARGN})
    add_test (NAME ${test_name} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${test_name} WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endmacro()

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib64)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib64)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(USE_CLANG "Use clang instead of gcc" OFF)

if(USE_CLANG)
    set(CMAKE_C_COMPILER   "clang")
    set(CMAKE_C_FLAGS   "-Werror -Wno-unused-private-field -Wno-unused-const-variable -Wno-strict-overflow  -fno-strict-overflow -g -O2 -Wno-deprecated-declarations -U__GNUC_MINOR__ -D__GNUC_MINOR__=9  -Wall -std=c99 -pthread -D_FILE_OFFSET_BITS=64 -fpic -fPIC -m64 -DTARGET_64 -D__STDC_LIMIT_MACROS" CACHE STRING "" FORCE)

    set(CMAKE_CXX_COMPILER   "clang++")
    set(CMAKE_CXX_FLAGS   "-Werror -Wno-unused-private-field -Wno-unused-const-variable -Wno-strict-overflow -fno-strict-overflow -g -O2 -Wno-deprecated-declarations -U__GNUC_MINOR__ -D__GNUC_MINOR__=9  -Wall -std=c++11 -pthread -D_FILE_OFFSET_BITS=64 -fpic -fPIC -m64 -DTARGET_64 -D__STDC_LIMIT_MACROS" CACHE STRING "" FORCE)
else()
    set(CMAKE_C_COMPILER   "gcc")
    set(CMAKE_C_FLAGS   "-Werror -Wno-unused-function -Wno-unused-variable -Wno-strict-overflow -Wno-unused-but-set-variable -fno-strict-overflow -g -O2 -Wall -std=c99 -pthread -D_FILE_OFFSET_BITS=64 -fpic -fPIC -m64 -DTARGET_64 -D__STDC_LIMIT_MACROS" CACHE STRING "" FORCE)

    set(CMAKE_CXX_COMPILER   "g++")
    set(CMAKE_CXX_FLAGS   "-Werror -Wno-unused-function -Wno-unused-variable -Wno-strict-overflow -Wno-unused-but-set-variable -fno-strict-overflow -g -O2 -Wall -std=c++11 -pthread -D_FILE_OFFSET_BITS=64 -fpic -fPIC -m64 -DTARGET_64 -D__STDC_LIMIT_MACROS"  CACHE STRING "" FORCE)
endif(USE_CLANG)
#add_compile_options(-Werror -Wno-unused-variable -Wno-strict-overflow -fno-strict-overflow -g -O1 -fsanitize=address -fno-omit-frame-pointer -Wno-deprecated-declarations -U__GNUC_MINOR__ -D__GNUC_MINOR__=9  -Wall -std=c++11 -pthread -D_FILE_OFFSET_BITS=64 -fpic -fPIC -m64 -DTARGET_64 -D__STDC_LIMIT_MACROS)
#add_link_options(-fsanitize=address)


set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(NOT COMPILER_SUPPORTS_CXX11)
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

string (REGEX MATCH "alios[1-9]+" os_version_suffix ${CMAKE_SYSTEM})

cmake_minimum_required(VERSION 3.14.5)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_ROOT}/cmake_modules")
include(Utils)

set(GIT_REF "unknown")
set(GIT_REV "unknown")
set(GIT_DATE "unknown")
set(GIT_LOG "unknown")
set(BUILD_TIME "unknown")
set(BUILD_HOST "unknown")
set(BUILD_USER "unknown")
set(BUILDENV_RPM "unknown")

GIT_REF_REV(${PROJECT_ROOT})
configure_file(${VERSION_FILE_IN} ${VERSION_FILE_OUT})

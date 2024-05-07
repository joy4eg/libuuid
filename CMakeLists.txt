cmake_minimum_required(VERSION 3.10)

project(uuid C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(LIBUUID_VERSION "1.0.4")

option(UUID_BUILD_LIBRARY_SHARED "Build shared library"   ON)
option(UUID_BUILD_LIBRARY_STATIC "Build static library"   OFF)
option(UUID_BUILD_TEST           "Build test application" ON)

set(SOURCE_FILES
    clear.c
    compare.c
    copy.c
    gen_uuid.c
    isnull.c
    pack.c
    parse.c
    randutils.c
    unpack.c
    unparse.c
    uuid_time.c
)

MESSAGE(STATUS "Build shared library:          " ${UUID_BUILD_LIBRARY_SHARED})
MESSAGE(STATUS "Build static library:          " ${UUID_BUILD_LIBRARY_STATIC})
MESSAGE(STATUS "Build test app:                " ${UUID_BUILD_TEST})

add_library(uuid OBJECT ${SOURCE_FILES})
set_target_properties(uuid PROPERTIES POSITION_INDEPENDENT_CODE ON)

include(GNUInstallDirs)
include(CheckIncludeFile)

check_include_file(fcntl.h HAVE_FCNTL_H)
check_include_file(inttypes.h HAVE_INTTYPES_H)
check_include_file(limits.h HAVE_LIMITS_H)
check_include_file(netinet/in.h HAVE_NETINET_IN_H)
check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(string.h HAVE_STRING_H)
check_include_file(sys/file.h HAVE_SYS_FILE_H)
check_include_file(sys/ioctl.h HAVE_SYS_IOCTL_H)
check_include_file(sys/socket.h HAVE_SYS_SOCKET_H)
check_include_file(sys/syscall.h HAVE_SYS_SYSCALL_H)
check_include_file(sys/random.h HAVE_SYS_RANDOM_H)
check_include_file(sys/time.h HAVE_SYS_TIME_H)
check_include_file(unistd.h HAVE_UNISTD_H)

include(CheckFunctionExists)

check_function_exists(ftruncate HAVE_FTRUNCATE)
check_function_exists(gettimeofday HAVE_GETTIMEOFDAY)
check_function_exists(memset HAVE_MEMSET)
check_function_exists(socket HAVE_SOCKET)
check_function_exists(strtoul HAVE_STRTOUL)
check_function_exists(usleep HAVE_USLEEP)
check_function_exists(srandom HAVE_SRANDOM)
check_function_exists(getrandom HAVE_GETRANDOM)

include(CheckSymbolExists)

set(CMAKE_REQUIRED_QUIET 1)
check_include_file(sys/sysconf.h HAVE_SYS_SYSCONF_H)
set(CMAKE_REQUIRED_QUIET 0)
if(HAVE_SYS_SYSCONF_H)
    check_symbol_exists(_SC_HOST_NAME_MAX sys/sysconf.h HAVE__SC_HOST_NAME_MAX)
endif()

include(CheckTypeSize)

set(CMAKE_EXTRA_INCLUDE_FILES "stdint.h")

if(HAVE_INTTYPES_H)
    list(APPEND CMAKE_EXTRA_INCLUDE_FILES "inttypes.h")
endif()

if(HAVE_STDLIB_H)
    list(APPEND CMAKE_EXTRA_INCLUDE_FILES "stdlib.h")
endif()

set(CMAKE_REQUIRED_QUIET 1)
check_include_file(sys/stat.h HAVE_SYS_STAT_H)
set(CMAKE_REQUIRED_QUIET 0)
if(HAVE_SYS_STAT_H)
    list(APPEND CMAKE_EXTRA_INCLUDE_FILES "sys/stat.h")
endif()

check_type_size(int32_t INT32_T)
check_type_size(mode_t MODE_T)
check_type_size(size_t SIZE_T)
check_type_size(ssize_t SSIZE_T)
check_type_size(uint16_t UINT16_T)
check_type_size(uint32_t UINT32_T)
check_type_size(uint64_t UINT64_T)
check_type_size(uint8_t UINT8_T)

function(RemoveNotSetVarList listName)
    set(list)
    foreach(var ${ARGN})
        if(${var})
            list(APPEND list ${var})
        endif()
    endforeach(var)
    set(${listName} ${list} PARENT_SCOPE)
endfunction(RemoveNotSetVarList)

function(CheckAll)
    foreach(var ${ARGN})
        if(NOT var)
            message(FATAL_ERROR "${var} is false")
        endif()
    endforeach(var)
endfunction(CheckAll)

RemoveNotSetVarList(ExistHeaderList
    HAVE_FCNTL_H
    HAVE_INTTYPES_H
    HAVE_LIMITS_H
    HAVE_NETINET_IN_H
    HAVE_STDLIB_H
    HAVE_STRING_H
    HAVE_SYS_FILE_H
    HAVE_SYS_IOCTL_H
    HAVE_SYS_SOCKET_H
    HAVE_SYS_SYSCALL_H
    HAVE_SYS_RANDOM_H
    HAVE_SYS_TIME_H
    HAVE_UNISTD_H
)

RemoveNotSetVarList(ExistFunctionList
    HAVE_FTRUNCATE
    HAVE_GETTIMEOFDAY
    HAVE_MEMSET
    HAVE_SOCKET
    HAVE_STRTOUL
    HAVE_USLEEP
    HAVE_SRANDOM
    HAVE_GETRANDOM
)

RemoveNotSetVarList(ExistMacroList
    HAVE__SC_HOST_NAME_MAX
)

CheckAll(
    HAVE_INT32_T
    HAVE_MODE_T
    HAVE_SIZE_T
    HAVE_SSIZE_T
    HAVE_UINT16_T
    HAVE_UINT32_T
    HAVE_UINT64_T
    HAVE_UINT8_T
)

target_compile_definitions(uuid PRIVATE _DEFAULT_SOURCE=1)
target_compile_definitions(uuid PRIVATE
    ${ExistHeaderList}
    ${ExistFunctionList}
    ${ExistMacroList}
)

target_include_directories(uuid INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:.>
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/uuid.pc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/uuid.pc"
    @ONLY
)

# Shared lib
if (UUID_BUILD_LIBRARY_SHARED)
    add_library(uuid_shared SHARED $<TARGET_OBJECTS:uuid>)
    set_target_properties(uuid_shared PROPERTIES OUTPUT_NAME uuid)
    install(TARGETS uuid_shared LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

# Static lib
if (UUID_BUILD_LIBRARY_STATIC)
    add_library(uuid_static STATIC $<TARGET_OBJECTS:uuid>)
    set_target_properties(uuid_static PROPERTIES OUTPUT_NAME uuid)
    install(TARGETS uuid_static LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

if (UUID_BUILD_TEST)
    add_executable(uuid_bin test_uuid.c)
    target_link_libraries(uuid_bin uuid)
    set_target_properties(uuid_bin PROPERTIES OUTPUT_NAME uuidtest)
endif()

if (UUID_BUILD_LIBRARY_STATIC OR UUID_BUILD_LIBRARY_SHARED)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/uuid.pc   DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
    install(FILES ${CMAKE_SOURCE_DIR}/uuid.h            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()

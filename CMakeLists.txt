project(BanBot)

cmake_minimum_required(VERSION 2.8)

set(boost_libs_DIR src/boost_1_45/libs)

#banbot sources for build
set(BanBot_SRCS
    src/Analyzer.cpp
    src/Backup.cpp
    src/ConfigLoader.cpp
    src/connection.cpp
    src/db.cpp
    src/FileLister.cpp
    src/handyFunctions.cpp
    src/InstructionsBlock.cpp
    src/logger.cpp
    src/main.cpp
    src/Scheduler.cpp
    src/server.cpp
)

#boost sources for buildign libs
set(Boost_Filesystem_SRCS
    ${boost_libs_DIR}/filesystem/v2/src/v2_operations.cpp
    ${boost_libs_DIR}/filesystem/v2/src/v2_path.cpp
    ${boost_libs_DIR}/filesystem/v2/src/v2_portability.cpp
)

set(Boost_SmartPtr_SRCS
    ${boost_libs_DIR}/smart_ptr/src/sp_collector.cpp
    ${boost_libs_DIR}/smart_ptr/src/sp_debug_hooks.cpp
)

set(Boost_System_SRCS
    ${boost_libs_DIR}/system/src/error_code.cpp
    ${boost_libs_DIR}/system/src/local_free_on_destruction.hpp
)

# for boost lib
add_library(boost_filesystem_LIB ${Boost_Filesystem_SRCS})
add_library(boost_smartPtr_LIB ${Boost_SmartPtr_SRCS})
add_library(boost_system_LIB ${Boost_System_SRCS})

# sqlite lib
add_subdirectory(src/sqlite3)

add_executable(BanBot ${BanBot_SRCS})

target_link_libraries(BanBot sqlite3_LIB boost_filesystem_LIB boost_smartPtr_LIB boost_system_LIB)

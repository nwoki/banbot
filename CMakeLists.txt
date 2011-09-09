project(BanBot)

cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(src/boost_1_45/)

# for debug
# add_definitions(-DDB_DEBUG -DDEBUG_MODE)

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

#boost libs
add_subdirectory(src/boost_1_45/libs/filesystem)
add_subdirectory(src/boost_1_45/libs/smart_ptr)
add_subdirectory(src/boost_1_45/libs/system)

# sqlite lib
add_subdirectory(src/sqlite3)

add_executable(BanBot ${BanBot_SRCS})

target_link_libraries(BanBot sqlite3_LIB boost_filesystem_LIB boost_smartPtr_LIB boost_system_LIB)

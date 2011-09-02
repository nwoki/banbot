#sqlite sources for build
set(sqlite3_SRCS
    sqlite3.c
)

# for compiling sqlite3 lib
add_definitions(-DSQLITE_THREADSAFE=0)

# sqlite lib
add_library(sqlite3_LIB ${sqlite3_SRCS})
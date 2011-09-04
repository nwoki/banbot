#filesystem lib sources for build
set(boostFilesystem_SRCS
    v2/src/v2_operations.cpp
    v2/src/v2_path.cpp
    v2/src/v2_portability.cpp
)

# sqlite lib
add_library(boost_filesystem_LIB ${boostFilesystem_SRCS})
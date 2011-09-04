#system lib sources for build
set(boostSystem_SRCS
    src/error_code.cpp
    src/local_free_on_destruction.hpp
)

# add lib
add_library(boost_system_LIB ${boostSystem_SRCS})
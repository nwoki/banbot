#smart_ptr lib sources for build
set(boostSmartPtr_SRCS
    src/sp_collector.cpp
    src/sp_debug_hooks.cpp
)

# add lib
add_library(boost_smartPtr_LIB ${boostSmartPtr_SRCS})
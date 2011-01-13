#build filesystem library
OBJECTS_PATH = BanbotLibs/

OBJECTS = $(OBJECTS_PATH)error_code.o \
	$(OBJECTS_PATH)sp_collector.o \
	$(OBJECTS_PATH)sp_debug_hooks.o \
	$(OBJECTS_PATH)v2_operations.o \
	$(OBJECTS_PATH)v2_path.o \
	$(OBJECTS_PATH)v2_portability.o 
	


#compiler
CPP = g++
CC = gcc
#====
DEFINES = 
#compiler flags
BOOST_INCPATH = -I"$(CURDIR)"
CFLAGS = -c -Wall $(DEFINES)$(BOOST_INCPATH)

#rules
all: $(OBJECTS)
	@ar r $(OBJECTS_PATH)libBanBot.a $(OBJECTS)
	
smart_ptr:
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)sp_collector.o libs/smart_ptr/src/sp_collector.cpp
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)sp_debug_hooks.o libs/smart_ptr/src/sp_debug_hooks.cpp
	
operations: 
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)v2_operations.o libs/filesystem/v2/src/v2_operations.cpp
#compile
BanbotLibs/error_code.o : libs/system/src/error_code.cpp
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)error_code.o libs/system/src/error_code.cpp
		
BanbotLibs/sp_collector.o : libs/smart_ptr/src/sp_collector.cpp
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)sp_collector.o libs/smart_ptr/src/sp_collector.cpp

BanbotLibs/sp_debug_hooks.o : libs/smart_ptr/src/sp_debug_hooks.cpp
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)sp_debug_hooks.o libs/smart_ptr/src/sp_debug_hooks.cpp

BanbotLibs/v2_operations.o : libs/filesystem/v2/src/v2_operations.cpp
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)v2_operations.o libs/filesystem/v2/src/v2_operations.cpp

BanbotLibs/v2_path.o : libs/filesystem/v2/src/v2_path.cpp
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)v2_path.o libs/filesystem/v2/src/v2_path.cpp
		
BanbotLibs/v2_portability.o : libs/filesystem/v2/src/v2_portability.cpp
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)v2_portability.o libs/filesystem/v2/src/v2_portability.cpp

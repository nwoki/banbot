#project files needed for executable
OBJECTS_PATH = src/.obj/

OBJECTS = $(OBJECTS_PATH)main.o \
	$(OBJECTS_PATH)db.o \
	$(OBJECTS_PATH)connection.o \
	$(OBJECTS_PATH)Analyzer.o \
	$(OBJECTS_PATH)ConfigLoader.o \
	$(OBJECTS_PATH)logger.o \
	$(OBJECTS_PATH)Backup.o \
	$(OBJECTS_PATH)server.o \
	$(OBJECTS_PATH)sqlite3.o \
	$(OBJECTS_PATH)InstructionsBlock.o \
	$(OBJECTS_PATH)Scheduler.o \
	$(OBJECTS_PATH)handyFunctions.o

# 	$(OBJECTS_PATH)FileLister.o


#compiler
CPP = g++
CC = gcc
#======defines=======
#for debug : -ggdb
# -DDEBUG_MODE
# -DDB_DEBUG( for database debug messages )
#
#for italian translations: -ggdb -DITA
#====optimization====
#for 32bit optimization: -m32 -pipe -march=x86-32
#for 64bit optimization: -m64 -pipe -march=x86-64
OPTIMIZ =
DEBUG =
DEFINES = $(OPTIMIZ) $(DEBUG)
#compiler flags
CFLAGS = -c -Wall $(DEFINES)

SQLITE3FLAGS = -DSQLITE_THREADSAFE=0

CLEAN_TARGETS =  src/*.h~ \
		 src/*.cpp~ \
		 src/sqlite3/*.h~ \
		 src/sqlite3/*.cpp~ \
		 BanBot \
		 src/.obj/*.o\
		 scripts/*~

BOOST_INCPATH = -Isrc/boost_1_45
#BOOST_LIBS = -Lsrc/boost_1_45/BanbotLibs/lib -lboost_filesystem -lboost_system

TARGET = BanBot

#rules__________
all: boost_build build_dir_check Makefile $(TARGET)

$(TARGET): boost_build build_dir_check obj_dir_chcek $(OBJECTS)
	@$(CPP) -o $(TARGET) $(OBJECTS) $(BOOST_LIBS)
	@echo ""
	@echo "BanBot ready for use ;)"

boost_build :
# 	@sh scripts/boostBuild.sh

boost_clean :
	@rm -rf src/boost_1_45/BanbotLibs
	@rm -rf src/boost_1_45/bin.v2
	@rm src/boost_1_45/bjam
	@rm -rf src/boost_1_45/project-config.jam*
	@echo "cleaned boost build"

build_dir_check :
# 	@sh scripts/objDirCheck.sh

clean :
	@rm -rf $(CLEAN_TARGETS)
	@echo "cleaned BanBot files"

obj_dir_chcek :
	@sh scripts/objDirCheck.sh

tar :
	@make clean
	@tar cjvf BanBot.tar.gz src/ cfg/ Makefile README GPL_License.txt
	@echo "BanBot.tar.gz archive created"

#single makes
main :	build_dir_check
	$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)main.o src/main.cpp

db :	build_dir_check
	$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)db.o src/db.cpp

connection :	build_dir_check
	$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)connection.o src/connection.cpp

analyzer :	build_dir_check
	$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)Analyzer.o src/Analyzer.cpp

configloader :	build_dir_check
	$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)ConfigLoader.o src/ConfigLoader.cpp

logger :	build_dir_check
	$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)logger.o src/logger.cpp

backup :	build_dir_check
	$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)Backup.o src/Backup.cpp

server :	build_dir_check
	$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)server.o src/server.cpp

sqlite3 :	build_dir_check
	$(CC) $(CFLAGS) $(SQLITE3FLAGS) -o $(OBJECTS_PATH)sqlite3.o src/sqlite3/sqlite3.c

instrucionsblock :	build_dir_check
	$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)InstructionsBlock.o src/InstructionsBlock.cpp

scheduler :	build_dir_check
	$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)Scheduler.o src/Scheduler.cpp

handyfunctions :	build_dir_check
	$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)handyFunctions.o src/handyFunctions.cpp
# filelister :	build_dir_check
# 	$(CPP) $(CFLAGS) $(BOOST_INCPATH) -o $(OBJECTS_PATH)FileLister.o src/FileLister.cpp

#compile_______

src/.obj/main.o : src/main.cpp
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)main.o src/main.cpp

src/.obj/db.o :	src/db.h src/db.cpp
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)db.o src/db.cpp

src/.obj/connection.o :	src/connection.h src/connection.cpp
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)connection.o src/connection.cpp

src/.obj/Analyzer.o :    src/Analyzer.h src/Analyzer.cpp
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)Analyzer.o src/Analyzer.cpp

src/.obj/ConfigLoader.o : src/ConfigLoader.h src/ConfigLoader.cpp
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)ConfigLoader.o src/ConfigLoader.cpp

src/.obj/logger.o	: src/logger.h src/logger.cpp
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)logger.o src/logger.cpp

src/.obj/Backup.o	: src/Backup.h src/Backup.cpp
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)Backup.o src/Backup.cpp

src/.obj/server.o	: src/server.cpp src/server.h
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)server.o src/server.cpp

src/.obj/sqlite3.o	: src/sqlite3/sqlite3.c src/sqlite3/sqlite3.h
		$(CC) $(CFLAGS) $(SQLITE3FLAGS) -o $(OBJECTS_PATH)sqlite3.o src/sqlite3/sqlite3.c

src/.obj/InstructionsBlock.o	: src/InstructionsBlock.h src/InstructionsBlock.cpp
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)InstructionsBlock.o src/InstructionsBlock.cpp

src/.obj/Scheduler.o	: src/Scheduler.h src/Scheduler.cpp
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)Scheduler.o src/Scheduler.cpp

src/.obj/handyFunctions.o	: src/handyFunctions.h src/handyFunctions.cpp
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)handyFunctions.o src/handyFunctions.cpp

# src/.obj/FileLister.o	: src/FileLister.h src/FileLister.cpp
# 	$(CPP) $(CFLAGS) $(BOOST_INCPATH) -o $(OBJECTS_PATH)FileLister.o src/FileLister.cpp


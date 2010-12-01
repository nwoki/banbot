#project files needed for executable
OBJECTS_PATH = src/obj/

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
	$(OBJECTS_PATH)Scheduler.o


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
		 src/obj/*.o

BOOST_BUILD = sh boostBuild.sh

TARGET = BanBot

#rules__________
all: Makefile $(TARGET)

$(TARGET):	$(OBJECTS)
	@$(CPP) $(OBJECTS) -o $(TARGET)
	@echo ""
	@echo "BanBot ready for use ;)"

clean_boost :
	@rm -r src/boost_1_45/BanbotLibs
	@rm -r src/boost_1_45/bin.v2
	@echo "cleaned boost build"

clean :
	@rm -rf $(CLEAN_TARGETS)
	@echo "cleaned BanBot files"

tar :
	@make clean
	@tar cjvf BanBot.tar.gz src/ cfg/ Makefile README GPL_License.txt
	@echo "BanBot.tar.gz archive created"

build_boost :
	@sh boostBuild.sh

#single makes
main :
	$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)main.o src/main.cpp

db :
	$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)db.o src/db.cpp

connection :
	$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)connection.o src/connection.cpp

analyzer :
	$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)Analyzer.o src/Analyzer.cpp

configloader :
	$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)ConfigLoader.o src/ConfigLoader.cpp

logger :
	$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)logger.o src/logger.cpp

backup :
	$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)Backup.o src/Backup.cpp

server :
	$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)server.o src/server.cpp

sqlite3 :
	$(CC) $(CFLAGS) $(SQLITE3FLAGS) -o $(OBJECTS_PATH)sqlite3.o src/sqlite3/sqlite3.c

instrucionsblock :
	$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)InstructionsBlock.o src/InstructionsBlock.cpp

scheduler :
	$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)Scheduler.o src/Scheduler.cpp

#compile_______

src/obj/main.o : src/main.cpp
	$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)main.o src/main.cpp

src/obj/db.o :	src/db.h src/db.cpp
	$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)db.o src/db.cpp

src/obj/connection.o :	src/connection.h src/connection.cpp
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)connection.o src/connection.cpp

src/obj/Analyzer.o :    src/Analyzer.h src/Analyzer.cpp
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)Analyzer.o src/Analyzer.cpp

src/obj/ConfigLoader.o : src/ConfigLoader.h src/ConfigLoader.cpp
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)ConfigLoader.o src/ConfigLoader.cpp

src/obj/logger.o	: src/logger.h src/logger.cpp
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)logger.o src/logger.cpp

src/obj/Backup.o	: src/Backup.h src/Backup.cpp
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)Backup.o src/Backup.cpp

src/obj/server.o	: src/server.cpp src/server.h
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)server.o src/server.cpp

src/obj/sqlite3.o	: src/sqlite3/sqlite3.c src/sqlite3/sqlite3.h
		$(CC) $(CFLAGS) $(SQLITE3FLAGS) -o $(OBJECTS_PATH)sqlite3.o src/sqlite3/sqlite3.c

src/obj/InstructionsBlock.o	: src/InstructionsBlock.h src/InstructionsBlock.cpp
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)InstructionsBlock.o src/InstructionsBlock.cpp

src/obj/Scheduler.o	: src/Scheduler.h src/Scheduler.cpp
		$(CPP) $(CFLAGS) -o $(OBJECTS_PATH)Scheduler.o src/Scheduler.cpp


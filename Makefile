#project files needed for executable
FILES = src/main.o \
	src/db.o \
	src/connection.o \
	src/Analyzer.o \
	src/ConfigLoader.o \
	src/logger.o \
	src/sqlite3/sqlite3.o

#compiler
CPP=g++
CC=gcc
#compiler flags
CFLAGS=-c -Wall -DSQLITE_THREADSAFE=0

BanBot : $(FILES)
	 $(CPP) $(FILES) -o BanBot

main.o : src/main.cpp
	 $(CPP) $(CFLAGS) src/main.cpp

db.o :	src/db.h src/db.cpp
	$(CPP) $(CFLAGS) src/db.h src/db.cpp

connection.o :	src/connection.h src/connection.cpp
		$(CPP) $(CFLAGS) src/connection.h src/connection.cpp

Analyzer.o :    src/Analyzer.h src/Analyzer.cpp
		$(CPP) $(CFLAGS) src/Analyzer.h src/Analyzer.cpp

ConfigLoader.o : src/ConfigLoader.h src/ConfigLoader.cpp
		 $(CPP) $(CFLAGS) src/ConfigLoader.h src/ConfigLoader.cpp

logger.o	: src/logger.h src/logger.cpp
		$(CPP) $(CFLAGS) src/logger.h src/logger.cpp

sqlite3.o	: src/sqlite3/sqlite3.c
		$(CC) $(CFLAGS) sqlite3.c

clean :
	 rm -rf src/*.o src/*.h~ src/*.cpp~
	 rm -rf src/sqlite3/*.o src/sqlite3/*.h~ src/sqlite3/*.cpp~
	 rm BanBot
	 echo "cleaned all makefiles"

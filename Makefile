#project files needed for executable
FILES = main.o \
	db.o \
	connection.o \
	Analyzer.o \
	ConfigLoader.o \
	logger.o \
	Backup.o \
	sqlite3.o

#compiler
CPP=g++
CC=gcc
#======defines=======
#for debug : -ggdb -DDEBUG_MODE
#for 32bit optimization: -m32
#for 64bit optimization: -m64
OPTIMIZ=-m64
DEBUG=-ggdb -DDEBUG_MODE
DEFINES=
#compiler flags
CFLAGS=-c -Wall $(DEFINES)

SQLITE3FLAGS = -DSQLITE_THREADSAFE=0

CLEAN_TARGETS=  src/*.gch \
		src/*.h~ \
		src/*.cpp~ \
		src/sqlite3/*.o \
		src/sqlite3/*.h~ \
		src/sqlite3/*.cpp~ \
		src/sqlite3/*.gch \
		BanBot \
		*.o

#default
BanBot :
	@make $(FILES)
	@$(CPP) $(FILES) -o BanBot
	@echo ""
	@echo "BanBot ready for use ;)"

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

Backup.o	: src/Backup.h src/Backup.cpp
		$(CPP) $(CFLAGS) src/Backup.h src/Backup.cpp

sqlite3.o	: src/sqlite3/sqlite3.c src/sqlite3/sqlite3.h
		$(CC) $(CFLAGS) $(SQLITE3FLAGS) src/sqlite3/sqlite3.c src/sqlite3/sqlite3.h

clean :
	@rm -rf $(CLEAN_TARGETS);
	@echo "cleaned all"

tar :
	@make clean
	@tar cjvf BanBot.tar.gz src/ cfg/ Makefile README GPL_License.txt
	@echo "BanBot.tar.gz archive created"


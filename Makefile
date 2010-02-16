#project files needed for executable
FILES = src/main.o src/db.o src/connection.o

#compiler
CC=g++

#compiler flags
CFLAGS=-c -Wall	

BanBot : $(FILES) 
	 $(CC) -lsqlite3 $(FILES) -o BanBot

main.o : src/main.cpp
	 $(CC) $(CFLAGS) src/main.cpp -o main.o

db.o :	src/db.h src/db.cpp
	$(CC) $(FLAGS) src/db.h src/db.cpp

connection.o :	src/connection.h src/connection.cpp
		$(CC) $(FLAGS) src/connection.h src/connection.cpp

clean :  
	 rm -rf src/*.o src/*.h~ src/*.cpp~ 
	 echo "cleaned all makefiles"
CC = gcc
FLAGS = -lsqlite3
SOURCE = database.h database.c package.h package.c main.c

default: $(SOURCE)
	$(CC) $(FLAGS) $^ -o ploc

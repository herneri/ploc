CC = gcc
FLAGS = -lsqlite3

default:
	$(CC) $(FLAGS) main.c -o ploc

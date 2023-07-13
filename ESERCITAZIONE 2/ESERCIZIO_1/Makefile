#Cartella per generatore random
RND:=../../RANDOM_GEN
CXXFLAGS:= -Wall -pedantic

CC = g++

compila: main.x

%.x: %.o random.o funzioni.o
	$(CC) $^ -o $@

%.o : %.cpp
	$(CC) ${CXXFLAGS} $< -c

random.o : ${RND}/random.cpp ${RND}/random.h
	$(CC) ${CXXFLAGS} $< -c

funzioni.o: funzioni.cpp funzioni.h ${RND}/random.h
	$(CC) ${CXXFLAGS} $< -c

clean :
	rm *.o *.x seed.out
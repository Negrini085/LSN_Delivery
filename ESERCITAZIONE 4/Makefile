#Cartella per generatore random
RND:=../RANDOM_GEN
CXXFLAGS:= -Wall -pedantic

CC = g++

compila: MD_MC.x

%.x: %.o random.o
	$(CC) $^ -o $@

%.o : %.cpp
	$(CC) ${CXXFLAGS} $< -c

random.o : ${RND}/random.cpp ${RND}/random.h
	$(CC) ${CXXFLAGS} $< -c



clean :
	rm *.o MD_MC.x

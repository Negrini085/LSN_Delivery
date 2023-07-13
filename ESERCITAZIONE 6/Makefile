#Cartella per generatore random
RND:=../RANDOM_GEN
CXXFLAGS:= -Wall -pedantic

CC = g++

compila: Monte_Carlo_ISING_1D.exe

%.exe : %.o random.o
	$(CC) $^ -o $@

%.o : %.cpp
	$(CC) ${CXXFLAGS} $< -c

random.o : ${RND}/random.cpp ${RND}/random.h
	$(CC) ${CXXFLAGS} $< -c


clean :
	rm *.o Monte_Carlo_ISING_1D.exe

#Cartella per generatore random
RND:=../RANDOM_GEN
CXXFLAGS:= -Wall -pedantic

CC = g++

compila: main.x

%.x: %.o random.o TravelingSalesman.o
	$(CC) $^ -o $@

%.o : %.cpp
	$(CC) ${CXXFLAGS} $< -c

random.o : ${RND}/random.cpp ${RND}/random.h
	$(CC) ${CXXFLAGS} $< -c

TravelingSalesman.o: TravelingSalesman.cpp TravelingSalesman.h
	$(CC) ${CXXFLAGS} $< -c


clean :
	rm *.o main.x


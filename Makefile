#	Makefile

all:	centralizedAlgorithm

CC = g++

centralizedAlgorithm:	centralizedAlgorithm.cpp
	$(CC) -std=c++14 -pthread -o centralizedAlgorithm centralizedAlgorithm.cpp 
 	
clean: 
	rm -f centralizedAlgorithm
	~

CC=gcc
CXX=g++
RM=rm

SRCS=src/insertion.cpp src/mergesort.cpp

all: sorts

sorts: $(SRCS)
	mkdir -p bin
	g++ -o bin/insertion src/insertion.cpp
	g++ -o bin/merge src/mergesort.cpp

clean:
	$(RM) -rf bin

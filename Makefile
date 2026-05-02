TARGET = parser

CXX=g++
CXXFLAGS=-std=c++20

SRCS = $(wildcard *.cpp)

.PHONY: all
all: ${SRCS}
	${CXX} ${CXXFLAGS} ${SRCS} -o ${TARGET}

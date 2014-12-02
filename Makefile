# Lab 4: Networking with PPP and PPM
# Christopher Jordan and Aaron Davis
# CSCI 4273/5273 Fall 2014

CFLG=-Wall -O0 -w -g -std=c++11
LIBS=-lpthread
CC=g++

# Main target
all: ppm_host_a ppm_host_b ppp_host_a ppp_host_b

# Compile rules
.c.o:
	gcc -c $(CFLG) $<
.cpp.o:
	g++ -c $(CFLG) $<

ppm_test:ppm_test.o
	$(CC) -o $@ $(CFLG) ppm.cpp ThreadPool.cpp Message.cpp $^ $(LIBS)

ppm_host_a:ppm_host_a.o
	$(CC) -o $@ $(CFLG) ppm.cpp ThreadPool.cpp Message.cpp $^ $(LIBS)

ppm_host_b:ppm_host_b.o
	$(CC) -o $@ $(CFLG) ppm.cpp ThreadPool.cpp Message.cpp $^ $(LIBS)

ppp_test:ppp_test.o
	$(CC) -o $@ $(CFLG) ppp.cpp ThreadPool.cpp Message.cpp $^ $(LIBS)

ppp_host_a:ppp_host_a.o
	$(CC) -o $@ $(CFLG) ppp.cpp ThreadPool.cpp Message.cpp $^ $(LIBS)

ppp_host_b:ppp_host_b.o
	$(CC) -o $@ $(CFLG) ppp.cpp ThreadPool.cpp Message.cpp $^ $(LIBS)	

clean:
	rm -f ppp_test ppm_test ppm_host_a ppm_host_b ppp_host_a ppp_host_b *.o *.a
# General build script

CXX=g++
STRIP=strip
FLAGS=-masm=intel -m32 -O3

all:
	$(CXX) $(FLAGS) -c constlib.cpp -o constlib.o
	$(CXX) $(FLAGS) -c libgen.cpp -o libgen.o
	$(CXX) $(FLAGS) -c postlink.cpp -o postlink.o
	$(CXX) $(FLAGS) constlib.o libgen.o postlink.o -o postlink
	$(STRIP) -s postlink

clean:
	-rm -f *.o
	-rm -f postlink

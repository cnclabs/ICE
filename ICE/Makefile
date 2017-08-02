CC = g++
CXX = g++
CPPFLAGS = -std=c++11 -fPIC -fopenmp -lm -O3
OBJECTS = vvNet.o ICE.o

ice: $(OBJECTS)
	ar rcs ./libvvNet.a $(OBJECTS)
	$(CC) $(CPPFLAGS) main.cpp -L./ -lvvNet -lboost_random -o ice
python:
	CC=$(CC) CXX=$(CXX) python3 setup.py build_ext -i
clean:
	rm ./ice
	rm $(OBJECTS)
	rm ./libvvNet.a

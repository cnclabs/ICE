CC = g++-4.9
CXX = g++-4.9
CPPFLAGS = -std=c++11 -fPIC -fopenmp -lm -O3
OBJECTS = vvNet.o ICE.o

cli: $(OBJECTS)
	ar rcs ./libvvNet.a $(OBJECTS)
	$(CC) $(CPPFLAGS) cli.cpp -L./ -lvvNet -lboost_random -o cli
python:
	CC=$(CC) CXX=$(CXX) python3 setup.py build_ext -i
clean:
	rm $(OBJECTS)
	rm ./libvvNet.a
	rm *.so

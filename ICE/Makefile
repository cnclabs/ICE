CC = g++-4.9
CPPFLAGS = -std=c++11 -fPIC -fopenmp -lm -O3
OBJECTS = vvNet.o ICE.o

all: $(OBJECTS)
	ar rcs ./libvvNet.a $(OBJECTS)
	$(CC) $(CPPFLAGS) cli.cpp -L./ -lvvNet -lboost_random -o cli

clean:
	rm $(OBJECTS)
	rm ./libvvNet.a
	rm ./cli

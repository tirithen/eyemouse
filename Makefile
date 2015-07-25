CXX = g++
CXXFLAGS = -ggdb

all:
	$(CXX) $(CXXFLAGS) `pkg-config --cflags opencv` -o eyemouse main.cpp `pkg-config --libs opencv`

release:
	$(CXX) $(CXXFLAGS) -O3 `pkg-config --cflags opencv` -o eyemouse main.cpp `pkg-config --libs opencv`

clean:
	rm -f eyemouse

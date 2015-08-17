CXX = g++
CXXFLAGS = -ggdb


all: Tracker.o EyeTracker.o Main.o
	$(CXX) Main.o Tracker.o EyeTracker.o `pkg-config --libs opencv` -o eyemouse


Main.o: Main.cpp
	$(CXX) -c `pkg-config --cflags opencv` Main.cpp

Tracker.o: Tracker.cpp Tracker.h
	$(CXX) -c `pkg-config --cflags opencv` Tracker.cpp

EyeTracker.o: EyeTracker.cpp EyeTracker.h
	$(CXX) -c `pkg-config --cflags opencv` EyeTracker.cpp

clean:
	rm -f eyemouse *.o

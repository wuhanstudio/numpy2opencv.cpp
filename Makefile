CXX 		= g++
CXXFLAGS	:= -I /usr/include/opencv4 -std=c++11

all: numpy2opencv

numpy2opencv: main.o
	g++ -Ofast -o $@ $^ `pkg-config --libs opencv4 libjpeg`

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

clean:
	rm numpy2opencv *.o
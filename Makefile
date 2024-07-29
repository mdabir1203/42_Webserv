CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++98
SRCS = src/main.cpp src/server.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = webserver

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
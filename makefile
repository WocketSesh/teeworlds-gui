CXX = g++
CXXFLAGS = -std=c++11 -Wall
GTKMMFLAGS = `pkg-config gtkmm-3.0 --cflags --libs`

SRC_DIR = src
SRCS = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*.h)
OBJS = $(SRCS:.cpp=.o)
EXECUTABLE = teeworlds

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(EXECUTABLE) $(GTKMMFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(GTKMMFLAGS)

clean:
	rm -f $(OBJS) $(EXECUTABLE)


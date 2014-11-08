
EXEC=CatchSelfTest
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

CXX = g++
CXXFLAGS  = -I../../include -I../../include/internal

$(EXEC): $(OBJECTS)
	$(CXX) -o $@ $^

clean:
	$(RM) $(OBJECTS)
	$(RM) $(EXEC)

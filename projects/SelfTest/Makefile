SOURCES		= ApproxTests.cpp ClassTests.cpp ConditionTests.cpp \
		  ExceptionTests.cpp GeneratorTests.cpp MessageTests.cpp \
		  MiscTests.cpp TestMain.cpp TrickyTests.cpp \
		  catch_self_test.cpp
OBJECTS		= $(patsubst %.cpp, %.o, $(SOURCES))

CXXFLAGS	= -I../../include

main: $(OBJECTS)
	$(CXX) -o $@ $^

clean:
	rm -f $(OBJECTS) main

.PHONY: main clean

SOURCES		= ApproxTests.cpp ClassTests.cpp ConditionTests.cpp \
		  ExceptionTests.cpp GeneratorTests.cpp MessageTests.cpp \
		  MiscTests.cpp TestMain.cpp TrickyTests.cpp \
		  catch_self_test.cpp
OBJECTS		= $(patsubst %.cpp, %.o, $(SOURCES))

CXXFLAGS	= -I../../include

main: $(OBJECTS)
	$(CXX) -o $@ $^

junit: main
	./main -r junit > junit.xml

clean:
	rm -f $(OBJECTS) main junit.xml

.PHONY: main junit

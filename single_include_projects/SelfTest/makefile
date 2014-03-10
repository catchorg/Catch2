SOURCES =	ApproxTests.cpp \
      		BDDTests.cpp \
			ClassTests.cpp \
			ConditionTests.cpp \
      		ExceptionTests.cpp \
      		GeneratorTests.cpp \
      		MessageTests.cpp \
      		MiscTests.cpp \
      		TestMain.cpp \
      		TrickyTests.cpp \
      		VariadicMacrosTests.cpp
OBJECTS    = $(patsubst %.cpp, %.o, $(SOURCES))
CXX = g++
CXXFLAGS  = -I../../single_include

CatchSelfTest: $(OBJECTS)
	$(CXX) -o $@ $^

clean:
	rm -f CatchSelfTest CatchSelfTest.exe $(OBJECTS)

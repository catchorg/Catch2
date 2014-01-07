SOURCES =	ApproxTests.cpp \
			ClassTests.cpp \
			ConditionTests.cpp \
      		ExceptionTests.cpp \
      		GeneratorTests.cpp \
      		MessageTests.cpp \
      		MiscTests.cpp \
      		TestMain.cpp \
      		TrickyTests.cpp \
      		BDDTests.cpp \
      		VariadicMacrosTests.cpp
OBJECTS    = $(patsubst %.cpp, %.o, $(SOURCES))
CXX = g++
CXXFLAGS  = -I../../include

CatchSelfTest: $(OBJECTS)
	$(CXX) -o $@ $^

clean:
	rm -f CatchSelfTest CatchSelfTest.exe $(OBJECTS)

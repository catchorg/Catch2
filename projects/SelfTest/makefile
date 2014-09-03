SOURCES = ApproxTests.cpp \
          ClassTests.cpp \
          ConditionTests.cpp \
          ExceptionTests.cpp \
          GeneratorTests.cpp \
          MessageTests.cpp \
          MiscTests.cpp \
          TestMain.cpp \
          TrickyTests.cpp \
          BDDTests.cpp \
          VariadicMacrosTests.cpp \
          EnumToString.cpp \
          ToStringPair.cpp \
          ToStringVector.cpp \
          ToStringWhich.cpp


OBJECTS    = $(patsubst %.cpp, %.o, $(SOURCES))
CXX = g++
CXXFLAGS  = -I../../include -std=c++11

CatchSelfTest: $(OBJECTS)
	$(CXX) -o $@ $^

test: CatchSelfTest
	./CatchSelfTest

clean:
	rm -f $(OBJECTS) CatchSelfTest


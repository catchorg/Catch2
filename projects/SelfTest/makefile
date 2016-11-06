SOURCES = ApproxTests.cpp \
          BDDTests.cpp \
          ClassTests.cpp \
          CmdLineTests.cpp \
          ConditionTests.cpp \
          EnumToString.cpp \
          ExceptionTests.cpp \
          GeneratorTests.cpp \
          MessageTests.cpp \
          MiscTests.cpp \
          PartTrackerTests.cpp \
          TagAliasTests.cpp \
          TestMain.cpp \
          ToStringPair.cpp \
          ToStringTuple.cpp \
          ToStringVector.cpp \
          ToStringWhich.cpp \
          TrickyTests.cpp \
          VariadicMacrosTests.cpp \


OBJECTS    = $(patsubst %.cpp, %.o, $(SOURCES))
CXX = g++
CXXFLAGS  = -I../../include -std=c++11

CatchSelfTest: $(OBJECTS)
	$(CXX) -o $@ $^

test: CatchSelfTest
	./CatchSelfTest

clean:
	rm -f $(OBJECTS) CatchSelfTest


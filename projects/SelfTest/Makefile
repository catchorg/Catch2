SOURCES		= ApproxTests.cpp BDDTests.cpp catch_self_test.cpp ClassTests.cpp \
	CmdLineTests.cpp ConditionTests.cpp ExceptionTests.cpp GeneratorTests.cpp \
	MessageTests.cpp MiscTests.cpp TestMain.cpp TrickyTests.cpp \
	VariadicMacrosTests.cpp

OBJECTS		= $(patsubst %.cpp, %.o, $(SOURCES))

CXXFLAGS	= -I../../include

catch_self_test: $(OBJECTS)
	$(CXX) -o $@ $^

clean:
	$(RM) $(OBJECTS) catch_self_test

check: catch_self_test
	./catch_self_test

.PHONY: clean check

import unittest

from catch_test_case  import TestCaseApprovedParser
from catch_test_case  import TestCaseResultParser
from catch_test_case  import TestCaseData
from catch_conditions import TestConditionData

class TestCaseTest(unittest.TestCase):

    def testTestCaseEquality(self):
		c1 = TestConditionData()
		c2 = TestConditionData()
		c1.filenamePrefix = "..\\..\\Test"
		c2.filenamePrefix = "..\\..\\Junk"
		t1 = TestCaseData()
		t2 = TestCaseData()
		t1.name = "Test 1"
		t2.name = "Test 1"
		t1.conditions.append(c1)
		t1.conditions.append(c2)
		t2.conditions.append(c1)
		t2.conditions.append(c2)
		self.assertTrue(t1 == t2)

		c3 = TestConditionData()
		c3.filenamePrefix = "..\\..\\Fail"
		t2.conditions.append(c3)
		self.assertFalse(t1 == t2)

		t1.conditions.append(c3)
		self.assertTrue(t1 == t2)

		t2.name = "Test 2"
		self.assertFalse(t1 == t2)

    def testEndOfTestCaseIsFound(self):
		obj = TestCaseApprovedParser()
		line = "-------------------------------------------------------------------------------"
		result = obj.parseApprovedLine(line)
		self.assertTrue(result == None)
		self.assertTrue(obj.current.empty())

    def testTestCaseNameIsFound(self):
		obj = TestCaseApprovedParser()
		lines = [ "-------------------------------------------------------------------------------",
					"./failing/TestClass/failingCase",
					"-------------------------------------------------------------------------------" ]
		for line in lines:
			result = obj.parseApprovedLine(line)
			self.assertTrue(result == None)
		self.assertTrue(obj.current.name == "./failing/TestClass/failingCase")
		self.assertTrue( not(obj.current.empty()) )

    def testTestCaseClassIsFound(self):
		obj = TestCaseApprovedParser()
		lines = [ "-------------------------------------------------------------------------------",
					"./failing/TestClass/failingCase",
					"-------------------------------------------------------------------------------",
					"ClassTests.cpp:<line number>",
					"..............................................................................." ]
		for line in lines:
			result = obj.parseApprovedLine(line)
			self.assertTrue(result == None)
		self.assertTrue(obj.current.filename == "ClassTests.cpp")
		self.assertTrue(obj.current.lineNumber == "line number")

    def testPartialConditionRequiresMoreData(self):
		obj = TestCaseApprovedParser()
		lines = [ "-------------------------------------------------------------------------------",
					"./failing/TestClass/failingCase",
					"-------------------------------------------------------------------------------",
					"ClassTests.cpp:<line number>",
					"...............................................................................",
					"ClassTests.cpp:<line number>: FAILED:" ]
		for line in lines:
			result = obj.parseApprovedLine(line)
			self.assertTrue(result == None)
		self.assertTrue(len(obj.current.conditions) == 0)

    def testTestCaseConditionIsFound(self):
		obj = TestCaseApprovedParser()
		lines = [ "-------------------------------------------------------------------------------",
					"./failing/TestClass/failingCase",
					"-------------------------------------------------------------------------------",
					"ClassTests.cpp:<line number>",
					"...............................................................................",
					"",
					"ClassTests.cpp:<line number>: FAILED:",
					"  REQUIRE( s == \"world\" )",
					"with expansion:",
					"  \"hello\" == \"world\""]
		for line in lines:
			result = obj.parseApprovedLine(line)
			self.assertTrue(result == None)
		self.assertTrue(not(obj.conditionParser.current.empty()))
		self.assertTrue(obj.conditionParser.current.reason == "FAILED")

    def testTestCaseConditionIsInsertedIntoList(self):
		obj = TestCaseApprovedParser()
		lines = [ "-------------------------------------------------------------------------------",
					"./failing/TestClass/failingCase",
					"-------------------------------------------------------------------------------",
					"ClassTests.cpp:<line number>",
					"...............................................................................",
					"",
					"ClassTests.cpp:<line number>: FAILED:",
					"  REQUIRE( s == \"world\" )",
					"with expansion:",
					"  \"hello\" == \"world\"",
					"",
					"-------------------------------------------------------------------------------"]
		for line in lines:
			result = obj.parseApprovedLine(line)
		
		self.assertTrue(isinstance(result, TestCaseData))
		self.assertTrue(len(result.conditions) > 0)
		self.assertTrue(result.conditions[0].filename == "ClassTests.cpp" )
		self.assertTrue(result.conditions[0].lineNumber == "line number" )
		self.assertTrue(result.conditions[0].reason == "FAILED" )
		newLines = result.generateApprovedLines()
		newLines.append("-------------------------------------------------------------------------------")
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testTwoTestCases(self):
		obj = TestCaseApprovedParser()
		lines = [ "-------------------------------------------------------------------------------",
					"./failing/TestClass/failingCase",
					"-------------------------------------------------------------------------------",
					"ClassTests.cpp:<line number>",
					"...............................................................................",
					"",
					"ClassTests.cpp:<line number>: FAILED:",
					"  REQUIRE( s == \"world\" )",
					"with expansion:",
					"  \"hello\" == \"world\"",
					"",
					"-------------------------------------------------------------------------------",
					"./failing/Fixture/failingCase",
					"-------------------------------------------------------------------------------",
					"ClassTests.cpp:<line number>",
					"...............................................................................",
					"",
					"ClassTests.cpp:<line number>: FAILED:",
					"  REQUIRE( m_a == 2 )",
					"with expansion:",
					"  1 == 2",
					"",
					"==============================================================================="
					]
		newLines = []
		for line in lines:
			result = obj.parseApprovedLine(line)
			if isinstance(result, TestCaseData):
				newLines += result.generateApprovedLines()

		newLines.append("===============================================================================")
		#for line in newLines:
		#	print line
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testTestCaseMultiConditionMatches(self):
		obj = TestCaseApprovedParser()
		lines = [ "-------------------------------------------------------------------------------",
			"./failing/conditions/equality",
			"-------------------------------------------------------------------------------",
			"ConditionTests.cpp:<line number>",
			"...............................................................................",
			"",
			"ConditionTests.cpp:<line number>: FAILED:",
			"  CHECK( data.int_seven == 6 )",
			"with expansion:",
			"  7 == 6",
			"",
			"ConditionTests.cpp:<line number>: FAILED:",
			"  CHECK( data.int_seven == 8 )",
			"with expansion:",
			"  7 == 8",
			"",
			"ConditionTests.cpp:<line number>: FAILED:",
			"  CHECK( data.int_seven == 0 )",
			"with expansion:",
			"  7 == 0",
			"",
			"ConditionTests.cpp:<line number>: FAILED:",
			"  CHECK( data.float_nine_point_one == Approx( 9.11f ) )",
			"with expansion:",
			"  9.1 == Approx( 9.11 )",
			"",
			"ConditionTests.cpp:<line number>: FAILED:",
			"  CHECK( data.float_nine_point_one == Approx( 9.0f ) )",
			"with expansion:",
			"  9.1 == Approx( 9 )",
			"",
			"ConditionTests.cpp:<line number>: FAILED:",
			"  CHECK( data.float_nine_point_one == Approx( 1 ) )",
			"with expansion:",
			"  9.1 == Approx( 1 )",
			"",
			"ConditionTests.cpp:<line number>: FAILED:",
			"  CHECK( data.float_nine_point_one == Approx( 0 ) )",
			"with expansion:",
			"  9.1 == Approx( 0 )",
			"",
			"ConditionTests.cpp:<line number>: FAILED:",
			"  CHECK( data.double_pi == Approx( 3.1415 ) )",
			"with expansion:",
			"  3.1415926535 == Approx( 3.1415 )",
			"",
			"ConditionTests.cpp:<line number>: FAILED:",
			"  CHECK( data.str_hello == \"goodbye\" )",
			"with expansion:",
			"  \"hello\" == \"goodbye\"",
			"",
			"ConditionTests.cpp:<line number>: FAILED:",
			"  CHECK( data.str_hello == \"hell\" )",
			"with expansion:",
			"  \"hello\" == \"hell\"",
			"",
			"ConditionTests.cpp:<line number>: FAILED:",
			"  CHECK( data.str_hello == \"hello1\" )",
			"with expansion:",
			"  \"hello\" == \"hello1\"",
			"",
			"ConditionTests.cpp:<line number>: FAILED:",
			"  CHECK( data.str_hello.size() == 6 )",
			"with expansion:",
			"  5 == 6",
			"",
			"ConditionTests.cpp:<line number>: FAILED:",
			"  CHECK( x == Approx( 1.301 ) )",
			"with expansion:",
			"  1.3 == Approx( 1.301 )",
			"",
			"==============================================================================="
			]

		for line in lines:
			result = obj.parseApprovedLine(line)

		self.assertTrue(isinstance(result, TestCaseData))
		newLines = result.generateApprovedLines()
		newLines.append("===============================================================================")
		#for line in newLines:
		#	print line
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testOneSection(self):
		obj = TestCaseApprovedParser()
		lines = [ "-------------------------------------------------------------------------------",
					"./failing/message/sections",
					"  one",
					"-------------------------------------------------------------------------------",
					"MessageTests.cpp:<line number>",
					"...............................................................................",
					"",
					"MessageTests.cpp:<line number>: FAILED:",
					"explicitly with message:",
					"  Message from section one",
					"",
					"==============================================================================="
					]
		newLines = []
		for line in lines:
			result = obj.parseApprovedLine(line)
			if isinstance(result, TestCaseData):
				newLines += result.generateApprovedLines()

		newLines.append("===============================================================================")
		#for line in newLines:
		#	print line
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testOneSection(self):
		obj = TestCaseApprovedParser()
		lines = [ "-------------------------------------------------------------------------------",
					"Comparisons between unsigned ints and negative signed ints match c++ standard",
					"behaviour",
					"-------------------------------------------------------------------------------",
					"ConditionTests.cpp:<line number>",
					"...............................................................................",
					"",
					"ConditionTests.cpp:<line number>:",
					"PASSED:",
					"  CHECK( ( -1 > 2u ) )",
					"with expansion:",
					"  true",
					"",
					"ConditionTests.cpp:<line number>:",
					"PASSED:",
					"  CHECK( -1 > 2u )",
					"with expansion:",
					"  -1 > 2",
					"",
					"==============================================================================="
					]
		newLines = []
		for line in lines:
			result = obj.parseApprovedLine(line)
			if isinstance(result, TestCaseData):
				newLines += result.generateApprovedLines()

		newLines.append("===============================================================================")
		#for line in newLines:
		#	print line
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testCaseBasicResults(self):
		obj = TestCaseResultParser()
		lines = [ "-------------------------------------------------------------------------------",
					"./failing/TestClass/failingCase",
					"-------------------------------------------------------------------------------",
					"..\..\..\SelfTest\ClassTests.cpp(34)",
					"...............................................................................",
					"",
					"..\..\..\SelfTest\ClassTests.cpp(28): FAILED:",
					"  REQUIRE( s == \"world\" )",
					"with expansion:",
					"  \"hello\" == \"world\"",
					"",
					"==============================================================================="
					]

		newLines = []
		for line in lines:
			result = obj.parseResultLine(line)
			if isinstance(result, TestCaseData):
				newLines += result.generateResultLines()

		newLines.append("===============================================================================")
		#for line in newLines:
		#	print line
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

if __name__ == '__main__':
    unittest.main()
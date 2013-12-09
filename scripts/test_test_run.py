import unittest
import catch_test_case

from catch_test_run  import TestRunData
from catch_test_run  import TestRunApprovedParser
from catch_test_run  import TestRunResultParser
from catch_test_case  import TestCaseData
from catch_conditions import TestConditionData

class TestCaseTest(unittest.TestCase):

    def testTestRunEquality(self):
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
		r1 = TestRunData()
		r2 = TestRunData()
		r1.appname = "One"
		r2.appname = "One"
		self.assertTrue(r1 == r2)

		r1.testcases.append(t1)
		self.assertFalse(r1 == r2)

		r2.testcases.append(t2)
		self.assertTrue(r1 == r2)

		c3 = TestConditionData()
		c3.filenamePrefix = "..\\..\\Fail"
		t2.conditions.append(c3)
		self.assertFalse(r1 == r2)

    def testStartOfTestRunIsFound(self):
		obj = TestRunApprovedParser()
		line = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
		result = obj.parseApprovedLine(line)
		self.assertTrue(result == None)

    def testTestRunVersionIsSet(self):
		obj = TestRunApprovedParser()
		lines = [ "",
					"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~",
					"CatchSelfTest is a <version> host application.",
					"Run with -? for options" ]
		for line in lines:
			result = obj.parseApprovedLine(line)
		#print obj
		self.assertTrue(result == None)
		self.assertTrue(obj.current.appname == "CatchSelfTest")
		self.assertTrue(obj.current.version == "<version>")

    def testTestRunParsesTestCase(self):
		obj = TestRunApprovedParser()
		lines = [ "",
					"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~",
					"CatchSelfTest is a <version> host application.",
					"Run with -? for options",
					"-------------------------------------------------------------------------------",
					"./failing/TestClass/failingCase",
					"-------------------------------------------------------------------------------",
					"ClassTests.cpp:<line number>",
					"...............................................................................",
					"ClassTests.cpp:<line number>: FAILED:",
					"  REQUIRE( s == \"world\" )",
					"with expansion:"
					"  \"hello\" == \"world\""]
		for line in lines:
			result = obj.parseApprovedLine(line)
		#print obj
		self.assertTrue(obj.testcaseParser.conditionParser.current.filename == "ClassTests.cpp" )
		self.assertTrue(obj.testcaseParser.conditionParser.current.lineNumber == "line number" )
		self.assertTrue(obj.testcaseParser.conditionParser.current.reason == "FAILED" )

    def testTestRunAddsTestCase(self):
		obj = TestRunApprovedParser()
		lines = [ "",
					"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~",
					"CatchSelfTest is a <version> host application.",
					"Run with -? for options",
					"-------------------------------------------------------------------------------",
					"./failing/TestClass/failingCase",
					"-------------------------------------------------------------------------------",
					"ClassTests.cpp:<line number>",
					"...............................................................................",
					"",
					"ClassTests.cpp:<line number>: FAILED:",
					"  REQUIRE( s == \"world\" )",
					"with expansion:"
					"  \"hello\" == \"world\"",
					"",
					"-------------------------------------------------------------------------------"
					]
		for line in lines:
			result = obj.parseApprovedLine(line)
		#print obj
		self.assertTrue( result == None )
		self.assertTrue( len(obj.current.testcases) == 1 )
		self.assertTrue(obj.current.testcases[0].filename == "ClassTests.cpp" )
		self.assertTrue(obj.current.testcases[0].lineNumber == "line number" )
		self.assertTrue( len(obj.current.testcases[0].conditions) == 1 )
		self.assertTrue(obj.current.testcases[0].conditions[0].filename == "ClassTests.cpp" )
		self.assertTrue(obj.current.testcases[0].conditions[0].lineNumber == "line number" )
		self.assertTrue(obj.current.testcases[0].conditions[0].reason == "FAILED" )

    def testTestRunParsesTwoTestCases(self):
		obj = TestRunApprovedParser()
		lines = [ "",
					"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~",
					"CatchSelfTest is a <version> host application.",
					"Run with -? for options",
					"",
					"-------------------------------------------------------------------------------",
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
					"===============================================================================",
					"122 test cases - 35 failed (753 assertions - 90 failed)",
					""
					]		
		testRun = None
		for line in lines:
			result = obj.parseApprovedLine(line)
			if isinstance(result, TestRunData):
				testRun = result
		
		self.assertTrue( isinstance(testRun, TestRunData) )
		newLines = testRun.generateApprovedLines()
		#for line in newLines:
		#	print "L:",line
		#print len(lines),",",len(newLines)
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testTestRunWithRandomOutput(self):
		obj = TestRunApprovedParser()
		lines = [ "",
					"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~",
					"CatchSelfTest is a <version> host application.",
					"Run with -? for options",
					"",
					"-------------------------------------------------------------------------------",
					"./failing/TestClass/failingCase",
					"-------------------------------------------------------------------------------",
					"ClassTests.cpp:<line number>",
					"...............................................................................",
					"",
					"MiscTests.cpp:<line number>: FAILED:",
					"explicitly with message:",
					"  to infinity and beyond",
					"",
					"Message from section one",
					"Message from section two",
					"Some information",
					"An error",
					"Message from section one",
					"Message from section two",
					"Some information",
					"An error",
					"hello",
					"hello",
					"===============================================================================",
					"122 test cases - 35 failed (753 assertions - 90 failed)",
					""
					]
		testRun = None
		for line in lines:
			result = obj.parseApprovedLine(line)
			if isinstance(result, TestRunData):
				testRun = result
		
		self.assertTrue( isinstance(testRun, TestRunData) )
		#print "O:",result.outputLine
		self.assertTrue( testRun.outputLine == 14 )
		newLines = testRun.generateApprovedLines()
		#for line in newLines:
		#	print line
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testRunBasicResult(self):
		obj = TestRunResultParser()
		lines = [ "Message from section one",
					"Message from section two",
					"Some information",
					"An error",
					"Message from section one",
					"Message from section two",
					"Some information",
					"An error",
					"hello",
					"hello",
					"",
					"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~",
					"TestCatch.exe is a Catch v1.0 b13 host application.",
					"Run with -? for options",
					"",
					"-------------------------------------------------------------------------------",
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
					"===============================================================================",
					"122 test cases - 35 failed (753 assertions - 90 failed)",
					""
					]

		testRun = None
		for line in lines:
			result = obj.parseResultLine(line)
			if isinstance(result, TestRunData):
				testRun = result
		
		self.assertTrue( isinstance(testRun, TestRunData) )
		#print "O:",testRun.outputLine
		self.assertTrue( testRun.outputLine == 0 )
		newLines = testRun.generateResultLines()
		#for line in newLines:
		#	print line
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testTestRunSorted(self):
		obj = TestRunApprovedParser()
		lines = [ "",
					"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~",
					"CatchSelfTest is a <version> host application.",
					"Run with -? for options",
					"",
					"-------------------------------------------------------------------------------",
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
					"===============================================================================",
					"122 test cases - 35 failed (753 assertions - 90 failed)",
					""
					]		
		testRun = None
		for line in lines:
			result = obj.parseApprovedLine(line)
			if isinstance(result, TestRunData):
				testRun = result
		
		self.assertTrue( isinstance(testRun, TestRunData) )
		newLines = testRun.generateSortedApprovedLines()
		#for line in newLines:
		#	print "L:",line
		#print len(lines),",",len(newLines)
		expectedLines = [ "",
					"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~",
					"CatchSelfTest is a <version> host application.",
					"Run with -? for options",
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
					"-------------------------------------------------------------------------------",
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
					"===============================================================================",
					"122 test cases - 35 failed (753 assertions - 90 failed)",
					""
					]		
		self.assertTrue( len(expectedLines) == len(newLines) )
		self.assertTrue( expectedLines == newLines )

if __name__ == '__main__':
    unittest.main()
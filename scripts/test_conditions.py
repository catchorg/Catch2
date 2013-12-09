import unittest
import catch_conditions

from catch_conditions import TestConditionApprovedParser
from catch_conditions import TestConditionResultParser
from catch_conditions import TestConditionData
from catch_conditions import RandomOutput

class ConditionTest(unittest.TestCase):

    def testConditionEquality(self):
		c1 = TestConditionData()
		c2 = TestConditionData()
		c1.state = TestConditionData.CONDITION
		c2.state = TestConditionData.EXPANSION
		c1.filenamePrefix = "..\\..\\Test"
		c2.filenamePrefix = "..\\..\\Test"
		self.assertTrue(c1 == c2)
		c2.filenamePrefix = "..\\..\\Junk"
		self.assertFalse(c1 == c2)
		self.assertTrue(c1 != c2)

    def testEndOfTestRunIsFound(self):
		obj = TestConditionApprovedParser()
		line = "==============================================================================="
		result = obj.parseApprovedLine(line)
		self.assertTrue(result == None)
		self.assertTrue(obj.current.empty())

    def testEndOfTestCaseIsFound(self):
		obj = TestConditionApprovedParser()
		line = "-------------------------------------------------------------------------------"
		result = obj.parseApprovedLine(line)
		self.assertTrue(result == None)
		self.assertTrue(obj.current.empty())

    def testFailedConditionIsFound(self):
		obj = TestConditionApprovedParser()
		line = "ClassTests.cpp:<line number>: FAILED:"
		result = obj.parseApprovedLine(line)
		self.assertTrue(result == None)
		self.assertTrue( not(obj.current.empty()) )

    def testOtherConditionIsFound(self):
		obj = TestConditionApprovedParser()
		line = "ClassTests.cpp:<line number>:"
		result = obj.parseApprovedLine(line)
		self.assertTrue(result == None)
		self.assertTrue( not(obj.current.empty()) )

    def testFailedConditionSetsReason(self):
		obj = TestConditionApprovedParser()
		line = "ClassTests.cpp:<line number>: FAILED:"
		result = obj.parseApprovedLine(line)
		self.assertTrue(obj.current.reason == "FAILED")
		self.assertTrue(obj.current.filename == "ClassTests.cpp")
		self.assertTrue(obj.current.lineNumber == "line number")

    def testOtherConditionSetsFileNameAndLine(self):
		obj = TestConditionApprovedParser()
		line = "MessageTests.cpp:<line number>:"
		result = obj.parseApprovedLine(line)
		self.assertTrue(obj.current.filename == "MessageTests.cpp")
		self.assertTrue(obj.current.lineNumber == "line number")

    def testFailedConditionSetsCondition(self):
		obj = TestConditionApprovedParser()
		lines = ["ClassTests.cpp:<line number>: FAILED:",
					"  REQUIRE( s == \"world\" )",
					""]
		for line in lines:
			result = obj.parseApprovedLine(line)
			self.assertTrue(result == None)
		
		self.assertTrue(obj.current.condition == "REQUIRE( s == \"world\" )")
		newLines = obj.current.generateApprovedLines()
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testExpansionConditionReturnsExpansion(self):
		obj = TestConditionApprovedParser()
		lines = ["ClassTests.cpp:<line number>: FAILED:",
					"  REQUIRE( s == \"world\" )",
					"with expansion:" ]
		for line in lines:
			result = obj.parseApprovedLine(line)
		self.assertTrue(result == None)

    def testExpansionSetsExpansion(self):
		obj = TestConditionApprovedParser()
		lines = [ "ClassTests.cpp:<line number>: FAILED:",
					"  REQUIRE( s == \"world\" )",
					"with expansion:",
					"  1 == 2",
					"",
					"-------------------------------------------------------------------------------"
					]
		for line in lines:
			result = obj.parseApprovedLine(line)
		#print lines
		self.assertTrue(isinstance(result, TestConditionData))
		self.assertTrue(len(result.expansion) == 1)
		self.assertTrue(result.expansion[0] == "1 == 2")
		newLines = result.generateApprovedLines()
		newLines.append("-------------------------------------------------------------------------------")
		#print newLines
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testTwoConditions(self):
		obj = TestConditionApprovedParser()
		lines = [ "ConditionTests.cpp:<line number>: FAILED:",
					"  CHECK( data.int_seven == 6 )",
					"with expansion:",
					"  7 == 6",
					"",
					"ConditionTests.cpp:<line number>: FAILED:",
					"  CHECK( data.int_seven == 8 )",
					"with expansion:",
					"  7 == 8",
					"",
					"-------------------------------------------------------------------------------"
					]
		newLines = []
		for line in lines:
			result = obj.parseApprovedLine(line)
			if isinstance(result, TestConditionData):
				newLines += result.generateApprovedLines()

		newLines.append("-------------------------------------------------------------------------------")
		#print lines
		#print newLines
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testSuccessConditions(self):
		obj = TestConditionApprovedParser()
		lines = [ "ApproxTests.cpp:<line number>:",
					"PASSED:",
					"  REQUIRE( d == Approx( 1.23 ) )",
					"with expansion:",
					"  1.23 == Approx( 1.23 )",
					"",
					"-------------------------------------------------------------------------------"
					]
		newLines = []
		for line in lines:
			result = obj.parseApprovedLine(line)
			if isinstance(result, TestConditionData):
				newLines += result.generateApprovedLines()

		newLines.append("-------------------------------------------------------------------------------")
		#print result
		#print lines
		#print newLines
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testConditionsWithoutExpansion(self):
		obj = TestConditionApprovedParser()
		lines = [ "ConditionTests.cpp:<line number>: FAILED:",
					"  CHECK( false != false )",
					"",
					"ConditionTests.cpp:<line number>: FAILED:",
					"  CHECK( true != true )",
					"",
					"ConditionTests.cpp:<line number>: FAILED:",
					"  CHECK( !true )",
					"with expansion:",
					"  false",
					"",
					"-------------------------------------------------------------------------------"
					]
		newLines = []
		for line in lines:
			result = obj.parseApprovedLine(line)
			if isinstance(result, TestConditionData):
				newLines += result.generateApprovedLines()

		newLines.append("-------------------------------------------------------------------------------")
		#print lines
		#print newLines
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testExceptionsExplicit(self):
		obj = TestConditionApprovedParser()
		lines = [ "ExceptionTests.cpp:<line number>: FAILED:",
					"  CHECK_THROWS_AS( thisThrows() )",
					"due to unexpected exception with message:",
					"  expected exception",
					"",
					"ExceptionTests.cpp:<line number>: FAILED:",
					"  CHECK_THROWS_AS( thisDoesntThrow() )",
					"because no exception was thrown where one was expected:",
					"",
					"ExceptionTests.cpp:<line number>: FAILED:",
					"  CHECK_NOTHROW( thisThrows() )",
					"due to unexpected exception with message:",
					"  expected exception",
					"",
					"-------------------------------------------------------------------------------"
					]
		newLines = []
		for line in lines:
			result = obj.parseApprovedLine(line)
			if isinstance(result, TestConditionData):
				newLines += result.generateApprovedLines()

		newLines.append("-------------------------------------------------------------------------------")
		#print lines
		#print newLines
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testExceptionsImplicit(self):
		obj = TestConditionApprovedParser()
		lines = [ "ExceptionTests.cpp:<line number>: FAILED:",
					"due to unexpected exception with message:",
					"  unexpected exception",
					"",
					"ExceptionTests.cpp:<line number>: FAILED:",
					"  {Unknown expression after the reported line}",
					"due to unexpected exception with message:",
					"  unexpected exception",
					"",
					"ExceptionTests.cpp:<line number>: FAILED:",
					"due to unexpected exception with message:",
					"  3.14",
					"",
					"-------------------------------------------------------------------------------"
					]
		newLines = []
		for line in lines:
			result = obj.parseApprovedLine(line)
			if isinstance(result, TestConditionData):
				#print result
				newLines += result.generateApprovedLines()

		newLines.append("-------------------------------------------------------------------------------")
		#print lines
		#print newLines
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testWarning(self):
		obj = TestConditionApprovedParser()
		lines = [ "MessageTests.cpp:<line number>:",
					"warning:",
					"  this is a warning",
					"",
					"-------------------------------------------------------------------------------"
					]
		newLines = []
		for line in lines:
			result = obj.parseApprovedLine(line)
			if isinstance(result, TestConditionData):
				#print result
				newLines += result.generateApprovedLines()

		newLines.append("-------------------------------------------------------------------------------")
		#print lines
		#print newLines
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testMessages1(self):
		obj = TestConditionApprovedParser()
		lines = [ "MessageTests.cpp:<line number>: FAILED:",
					"  REQUIRE( a == 1 )",
					"with expansion:",
					"  2 == 1",
					"with messages:",
					"  this message should be logged",
					"  so should this",
					"",
					"-------------------------------------------------------------------------------"
					]
		newLines = []
		for line in lines:
			result = obj.parseApprovedLine(line)
			if isinstance(result, TestConditionData):
				#print result
				newLines += result.generateApprovedLines()

		newLines.append("-------------------------------------------------------------------------------")
		#print lines
		#print newLines
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testMessagesExplicitFail(self):
		obj = TestConditionApprovedParser()
		lines = [ "MessageTests.cpp:<line number>: FAILED:",
					"explicitly with message:",
					"  This is a failure",
					"",
					"-------------------------------------------------------------------------------"
					]
		newLines = []
		for line in lines:
			result = obj.parseApprovedLine(line)
			if isinstance(result, TestConditionData):
				#print result
				newLines += result.generateApprovedLines()

		newLines.append("-------------------------------------------------------------------------------")
		#print lines
		#print newLines
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testMessagesOutput(self):
		obj = TestConditionApprovedParser()
		lines = [ "MessageTests.cpp:<line number>: FAILED:",
					"explicitly with message:",
					"  Message from section two",
					"",
					"Message from section one",
					"Message from section two",
					"-------------------------------------------------------------------------------"
					]
		newLines = []
		for line in lines:
			result = obj.parseApprovedLine(line)
			if isinstance(result, TestConditionData):
				#print result
				newLines += result.generateApprovedLines()

		newLines.append("-------------------------------------------------------------------------------")
		#print lines
		#print newLines
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testMultiMessages(self):
		obj = TestConditionApprovedParser()
		lines = [ "MessageTests.cpp:<line number>: FAILED:",
					"  REQUIRE( i < 10 )",
					"with expansion:",
					"  10 < 10",
					"with messages:",
					"  current counter 10",
					"  i := 10",
					"",
					"-------------------------------------------------------------------------------"
					]
		newLines = []
		for line in lines:
			result = obj.parseApprovedLine(line)
			if isinstance(result, TestConditionData):
				#print result
				newLines += result.generateApprovedLines()

		newLines.append("-------------------------------------------------------------------------------")
		#print lines
		#print newLines
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testMiscMessages(self):
		obj = TestConditionApprovedParser()
		lines = [ "MiscTests.cpp:<line number>: FAILED:",
					"  CHECK( ( fib[i] % 2 ) == 0 )",
					"with expansion:",
					"  1 == 0",
					"with message:",
					"  Testing if fib[0] (1) is even",
					"",
					"MiscTests.cpp:<line number>: FAILED:",
					"  CHECK( ( fib[i] % 2 ) == 0 )",
					"with expansion:",
					"  1 == 0",
					"with message:",
					"  Testing if fib[1] (1) is even",
					"",
					"MiscTests.cpp:<line number>: FAILED:",
					"  CHECK( ( fib[i] % 2 ) == 0 )",
					"with expansion:",
					"  1 == 0",
					"with message:",
					"  Testing if fib[3] (3) is even",
					"",
					"MiscTests.cpp:<line number>: FAILED:",
					"  CHECK( ( fib[i] % 2 ) == 0 )",
					"with expansion:",
					"  1 == 0",
					"with message:",
					"  Testing if fib[4] (5) is even",
					"",
					"MiscTests.cpp:<line number>: FAILED:",
					"  CHECK( ( fib[i] % 2 ) == 0 )",
					"with expansion:",
					"  1 == 0",
					"with message:",
					"  Testing if fib[6] (13) is even",
					"",
					"MiscTests.cpp:<line number>: FAILED:",
					"  CHECK( ( fib[i] % 2 ) == 0 )",
					"with expansion:",
					"  1 == 0",
					"with message:",
					"  Testing if fib[7] (21) is even",
					"",
					"Some information",
					"An error",
					"-------------------------------------------------------------------------------"
					]
		newLines = []
		for line in lines:
			result = obj.parseApprovedLine(line)
			if isinstance(result, TestConditionData):
				#print result
				newLines += result.generateApprovedLines()

		newLines.append("-------------------------------------------------------------------------------")
		#print lines
		#print newLines
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testRandomOutput(self):
		obj = TestConditionApprovedParser()
		lines = [ "MiscTests.cpp:<line number>: FAILED:",
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
					"-------------------------------------------------------------------------------"
					]
		newLines = []
		for line in lines:
			try:
				result = obj.parseApprovedLine(line)
			except RandomOutput as e:
				randomOutput = e.output
			if isinstance(result, TestConditionData):
				#print result
				newLines += result.generateApprovedLines()

		self.assertTrue( len(randomOutput) == 10)
		newLines += randomOutput
		newLines.append("-------------------------------------------------------------------------------")
		#print lines
		#print newLines
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testMultiLineWarning(self):
		obj = TestConditionApprovedParser()
		lines = [ "TrickyTests.cpp:<line number>:",
					"warning:",
					"  Uncomment the code in this test to check that it gives a sensible compiler",
					"  error",
					"",
					"-------------------------------------------------------------------------------"
					]
		newLines = []
		for line in lines:
			result = obj.parseApprovedLine(line)
			if isinstance(result, TestConditionData):
				#print result
				newLines += result.generateApprovedLines()

		newLines.append("-------------------------------------------------------------------------------")
		#print lines
		#print newLines
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testMultiMessagesAfterCondition(self):
		obj = TestConditionApprovedParser()
		lines = [ "MiscTests.cpp:<line number>: FAILED:",
					"  REQUIRE( false )",
					"with messages:",
					"  hi",
					"  i := 7",
					"",
					"-------------------------------------------------------------------------------"
					]
		newLines = []
		randomOutput = []
		for line in lines:
			result = obj.parseApprovedLine(line)
			if isinstance(result, TestConditionData):
				#print result
				newLines += result.generateApprovedLines()

		newLines.append("-------------------------------------------------------------------------------")
		#print lines
		#print newLines
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testNoAssertions(self):
		obj = TestConditionApprovedParser()
		lines = [ "",
					"No assertions in test case './succeeding/exceptions/implicit'",
					"",
					"-------------------------------------------------------------------------------"
					]
		newLines = []
		randomOutput = []
		for line in lines:
			result = obj.parseApprovedLine(line)
			if isinstance(result, TestConditionData):
				#print result
				newLines += result.generateApprovedLines()

		newLines.append("-------------------------------------------------------------------------------")
		#print lines
		#print newLines
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testNoAssertionsWithOutput(self):
		obj = TestConditionApprovedParser()
		lines = [ "",
					"No assertions in section 'one'",
					"",
					"Message from section two",
					"-------------------------------------------------------------------------------"
					]
		newLines = []
		randomOutput = []
		for line in lines:
			result = obj.parseApprovedLine(line)
			if isinstance(result, TestConditionData):
				#print result
				newLines += result.generateApprovedLines()

		newLines.append("-------------------------------------------------------------------------------")
		#print lines
		#print newLines
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testFailedButOk(self):
		obj = TestConditionApprovedParser()
		lines = [ "MessageTests.cpp:<line number>:",
					"FAILED - but was ok:",
					"  CHECK_NOFAIL( 1 == 2 )",
					"",
					"",
					"No assertions in test case './succeeding/nofail'",
					"",
					"-------------------------------------------------------------------------------"
					]
		newLines = []
		randomOutput = []
		for line in lines:
			result = obj.parseApprovedLine(line)
			if isinstance(result, TestConditionData):
				#print result
				newLines += result.generateApprovedLines()

		newLines.append("-------------------------------------------------------------------------------")
		#print lines
		#print newLines
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testMultiLineExpansion(self):
		obj = TestConditionApprovedParser()
		lines = [ "MiscTests.cpp:<line number>:",
					"PASSED:",
					"  CHECK_THAT( testStringForMatching() AllOf( Catch::Contains( \"string\" ), Catch::Contains( \"abc\" ) ) )",
					"with expansion:",
					"  \"this string contains 'abc' as a substring\" ( contains: \"string\" and",
					"  contains: \"abc\" )",
					"",
					"-------------------------------------------------------------------------------"
					]
		newLines = []
		randomOutput = []
		for line in lines:
			result = obj.parseApprovedLine(line)
			if isinstance(result, TestConditionData):
				#print result
				newLines += result.generateApprovedLines()

		newLines.append("-------------------------------------------------------------------------------")
		#print lines
		#print newLines
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testMultiLineExpansionWithWrap(self):
		obj = TestConditionApprovedParser()
		lines = [ "TestMain.cpp:<line number>:",
					"PASSED:",
					"  CHECK( text.toString() == \" one two\n    three\n    four\" )",
					"with expansion:",
					"  \" one two",
					"      three",
					"      four\"",
					"  ==",
					"  \" one two",
					"      three",
					"      four\"",
					"",
					"-------------------------------------------------------------------------------"
					]
		newLines = []
		randomOutput = []
		for line in lines:
			result = obj.parseApprovedLine(line)
			if isinstance(result, TestConditionData):
				#print result
				newLines += result.generateApprovedLines()

		newLines.append("-------------------------------------------------------------------------------")
		#print lines
		#print newLines
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testMultiLineExpansionWithTruncation(self):
		obj = TestConditionApprovedParser()
		lines = [ "TestMain.cpp:<line number>:",
					"PASSED:",
					"  CHECK_THAT( t.toString() EndsWith( \"... message truncated due to excessive size\" ) )",
					"with expansion:",
					"  \"***************************************************************************-",
					"  ***-",
					"  ****************************************************************************-",
					"  **-",
					"... message truncated due to excessive size",
					"",
					"-------------------------------------------------------------------------------"
					]
		newLines = []
		for line in lines:
			result = obj.parseApprovedLine(line)
			if isinstance(result, TestConditionData):
				#print result
				newLines += result.generateApprovedLines()

		newLines.append("-------------------------------------------------------------------------------")
		#print lines
		#print newLines
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testBasicResultsParser(self):
		obj = TestConditionResultParser()
		lines = [ "..\..\..\SelfTest\ClassTests.cpp(28): FAILED:",
					"  REQUIRE( s == \"world\" )",
					"with expansion:",
					"  \"hello\" == \"world\"",
					"",
					"-------------------------------------------------------------------------------"
					]
		newLines = []
		for line in lines:
			result = obj.parseResultLine(line)
			if isinstance(result, TestConditionData):
				#print result
				newLines += result.generateResultLines()

		newLines.append("-------------------------------------------------------------------------------")
		#print lines
		#print newLines
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

    def testBasicResultsWarning(self):
		obj = TestConditionResultParser()
		lines = [ "..\..\..\SelfTest\MessageTests.cpp(17): ",
					"warning:",
					"  this is a warning",
					"",
					"-------------------------------------------------------------------------------"
					]
		newLines = []
		for line in lines:
			result = obj.parseResultLine(line)
			if isinstance(result, TestConditionData):
				#print result
				newLines += result.generateResultLines()

		newLines.append("-------------------------------------------------------------------------------")
		#print lines
		#print newLines
		self.assertTrue( len(lines) == len(newLines) )
		self.assertTrue( lines == newLines )

if __name__ == '__main__':
    unittest.main()
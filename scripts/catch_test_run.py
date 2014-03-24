import re
import os

from catch_test_case  import TestCaseApprovedParser
from catch_test_case  import TestCaseResultParser
from catch_test_case  import TestCaseData
from catch_conditions import RandomOutput

class TestRunData:

	def __init__(self):
		self.appname = ""
		self.version = ""
		self.testcases = []
		self.results = ""
		self.output = []
		self.outputLine = 0
		self.writtenOutput = False
		self.sysout = []
		self.syserr = []
		self.errors = ""
		self.failures = ""
		self.tests = ""

	def __eq__(self, other):
		return self.__dict__ == other.__dict__

	def __repr__(self):
		result = "[" + self.appname + ", " + self.version + " [ "
		suffix = ""
		for case in self.testcases:
			result += suffix
			result += repr(case)
			suffix = ", "
		result += " ]"
		result += self.results
		result += " ]"
		return result

	def empty(self):
		if len(self.appname):
			return False
		return True

	def generateApprovedLines(self):
		if self.empty():
			raise Exception("Empty test run..." + repr(self))
		lines = []
		self.writtenOutput = False
		if not(self.writtenOutput) and len(self.output) > 0 and self.outputLine == 0:
			lines += self.output
			self.writtenOutput = True
		if len(self.appname):
			lines.append("")
			lines.append("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~")
			lines.append(self.appname + " is a " + self.version + " host application.")
			lines.append("Run with -? for options")
		lines.append("")
		for case in self.testcases:
			lines += case.generateApprovedLines()
			if not(self.writtenOutput) and len(self.output) > 0 and len(lines) >= self.outputLine:
				lines += self.output
				self.writtenOutput = True
		lines.append("===============================================================================")
		lines.append(self.results)
		lines.append("")
			
		return lines

	def generateSortedApprovedLines(self):
		if self.empty():
			raise Exception("Empty test run..." + repr(self))
		lines = []
		self.writtenOutput = False
		if not(self.writtenOutput) and len(self.output) > 0 and self.outputLine == 0:
			lines += self.output
			self.writtenOutput = True
		if len(self.appname):
			lines.append("")
			lines.append("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~")
			lines.append(self.appname + " is a " + self.version + " host application.")
			lines.append("Run with -? for options")
		lines.append("")
		sortedTestcases = sorted(self.testcases, key=lambda x: x.name, reverse=False)
		for case in sortedTestcases:
			lines += case.generateApprovedLines()
			if not(self.writtenOutput) and len(self.output) > 0 and len(lines) >= self.outputLine:
				lines += self.output
				self.writtenOutput = True
		lines.append("===============================================================================")
		lines.append(self.results)
		lines.append("")
			
		return lines

	def generateResultLines(self):
		if self.empty():
			raise Exception("Empty test run..." + repr(self))
		lines = []
		self.writtenOutput = False
		if not(self.writtenOutput) and len(self.output) > 0 and self.outputLine == 0:
			lines += self.output
			self.writtenOutput = True
		if len(self.appname):
			lines.append("")
			lines.append("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~")
			lines.append(self.appname + " is a " + self.version + " host application.")
			lines.append("Run with -? for options")
		lines.append("")
		for case in self.testcases:
			lines += case.generateResultLines()
			if not(self.writtenOutput) and len(self.output) > 0 and len(lines) >= self.outputLine:
				lines += self.output
				self.writtenOutput = True
		lines.append("===============================================================================")
		lines.append(self.results)
		lines.append("")
			
		return lines

	def generateUnapprovedLines(self, outputLine):
		if self.empty():
			raise Exception("Empty test run..." + repr(self))
		lines = []
		self.writtenOutput = False
		#print "U:",outputLine,",",self.output
		if not(self.writtenOutput) and len(self.output) > 0 and outputLine == 0:
			lines += self.output
			self.writtenOutput = True
		if len(self.appname):
			lines.append("")
			lines.append("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~")
			lines.append("CatchSelfTest" + " is a " + "<version>" + " host application.")
			lines.append("Run with -? for options")
		lines.append("")
		for case in self.testcases:
			lines += case.generateUnapprovedLines()
			if not(self.writtenOutput) and len(self.output) > 0 and len(lines) >= outputLine:
				lines += self.output
				self.writtenOutput = True
		lines.append("===============================================================================")
		lines.append(self.results)
		lines.append("")
			
		return lines

	def generateSortedUnapprovedLines(self, outputLine):
		if self.empty():
			raise Exception("Empty test run..." + repr(self))
		lines = []
		self.writtenOutput = False
		#print "U:",outputLine,",",self.output
		if not(self.writtenOutput) and len(self.output) > 0 and outputLine == 0:
			lines += self.output
			self.writtenOutput = True
		if len(self.appname):
			lines.append("")
			lines.append("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~")
			lines.append("CatchSelfTest" + " is a " + "<version>" + " host application.")
			lines.append("Run with -? for options")
		lines.append("")
		sortedTestcases = sorted(self.testcases, key=lambda x: x.name, reverse=False)
		for case in sortedTestcases:
			lines += case.generateUnapprovedLines()
			if not(self.writtenOutput) and len(self.output) > 0 and len(lines) >= outputLine:
				lines += self.output
				self.writtenOutput = True
		lines.append("===============================================================================")
		lines.append(self.results)
		lines.append("")
			
		return lines

	def generateSortedUnapprovedJunit(self):
		lines = []
		#print "U:",outputLine,",",self.output
		lines.append("<testsuites>")
		l = "  <testsuite name=\""
		l += self.appname
		l += "\" errors=\""
		l += self.errors
		l += "\" failures=\""
		l += self.failures
		l += "\" tests=\""
		l += self.tests
		l += "\" hostname=\"tbd\" time=\"{duration}\" timestamp=\"tbd\">"
		lines.append(l)
		sortedTestcases = sorted(self.testcases, key=lambda x: x.classname, reverse=False)
		sortedTestcases = sorted(sortedTestcases, key=lambda x: x.name, reverse=False)
		#sortedTestcases = self.testcases
		for case in sortedTestcases:
			lines += case.generateUnapprovedJunit()
			
		if len(self.sysout) > 0:
			lines.append("    <system-out>")
			for l in self.sysout:
				lines.append(l)
			lines.append("    </system-out>")
		if len(self.syserr) > 0:
			lines.append("    <system-err>")
			for l in self.syserr:
				lines.append(l)
			lines.append("    </system-err>")

		lines.append("  </testsuite>")
		lines.append("</testsuites>")
		return lines

	def generateSortedUnapprovedXml(self):
		lines = []
		#print "U:",outputLine,",",self.output
		lines.append("<Catch name=\"" + self.appname + "\">")
		lines.append("  <Group name=\"~_\">")
		
		sortedTestcases = sorted(self.testcases, key=lambda x: x.classname, reverse=False)
		sortedTestcases = sorted(sortedTestcases, key=lambda x: x.name, reverse=False)
		#sortedTestcases = self.testcases
		for case in sortedTestcases:
			lines += case.generateUnapprovedXml()
		
		l = "<OverallResults successes=\""
		#  successes="663" failures="109"
		l += self.tests
		l += "\" failures=\""
		l += self.failures
		l += "\"/>"
		lines.append("    " + l)
		lines.append("  </Group>")
		lines.append("  " + l)
		lines.append("</Catch>")
		return lines

	def addTestCase(self, name):
		testcase = TestCaseData()
		testcase.name = name
		testcase.nameParts.append(name)
		self.testcases.append(testcase)
		return testcase

	def addClassTestCase(self, cls, name):
		testcase = TestCaseData()
		testcase.classname = cls
		testcase.name = name
		testcase.nameParts.append(name)
		self.testcases.append(testcase)
		return testcase

	def addSysout(self, output):
		self.sysout = output

	def addSyserr(self, output):
		self.syserr = output

class TestRunApprovedParser:
	NONE = 0
	VERSION_EXPECTED = 1
	TEST_CASE_EXPECTED = 2
	END_RUN_INFO = 3

	versionParser = re.compile( r'(.*)is a (<version>*).*' )

	def __init__(self):
		self.state = self.NONE
		self.current = TestRunData()
		self.testcaseParser = TestCaseApprovedParser()
		self.lineNumber = 0

	def parseApprovedLine(self,line):
		result = None
		if self.state == self.NONE:
			if line.startswith("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"):
				self.state = self.VERSION_EXPECTED
			elif len(line):
				raise Exception("Unknown parse line: '" + line + "'")
		elif self.state == self.VERSION_EXPECTED:
			m = self.versionParser.match(line)
			if m:
				self.current.appname = m.group(1).strip()
				self.current.version = m.group(2).strip()
				self.state = self.TEST_CASE_EXPECTED
			elif len(line):
				raise Exception("Unknown parse line: '" + line + "'")
		elif self.state == self.TEST_CASE_EXPECTED:
			if line == "Run with -? for options":
				pass
			else:
				testcase = None
				try:
					testcase = self.testcaseParser.parseApprovedLine(line)
				except RandomOutput as e:
					#print "E:", self.lineNumber, ", ",e.output
					self.current.output = e.output
					self.current.outputLine = self.lineNumber - 10
				
				if isinstance(testcase, TestCaseData):
					self.current.testcases.append(testcase)
					if line.startswith("==============================================================================="):
						self.state = self.END_RUN_INFO
		elif self.state == self.END_RUN_INFO:
			if len(line):
				self.current.results = line.strip()
				result = self.current

		self.lineNumber += 1
		return result

class TestRunApprovedHandler:

	def __init__(self, filePath):
		rawFile = open( filePath, 'r' )
		parser = TestRunApprovedParser()
		lineNumber = 0
		self.current = None
		for line in rawFile:
			line = line.rstrip()
			#print "L:", lineNumber, "'",line,"'"
			result = parser.parseApprovedLine(line)
			if isinstance(result, TestRunData):
				self.current = result
			lineNumber += 1
		if not(isinstance(self.current, TestRunData) ):
			raise Exception("File could not be parsed: '" + filePath + "'")

	def writeRawFile(self,filePath):
		rawWriteFile = open( filePath, 'wb' )
		lines = self.current.generateApprovedLines()
		for line in lines:
			rawWriteFile.write(line + "\n")

	def writeSortedRawFile(self,filePath):
		rawWriteFile = open( filePath, 'wb' )
		lines = self.current.generateSortedApprovedLines()
		for line in lines:
			rawWriteFile.write(line + "\n")

class TestRunResultParser:
	NONE = 0
	VERSION_EXPECTED = 1
	TEST_CASE_EXPECTED = 2
	END_RUN_INFO = 3

	versionParser = re.compile( r'(.*)is a (Catch v[0-9]*.[0-9]* b[0-9]*).*' )

	def __init__(self):
		self.state = self.NONE
		self.current = TestRunData()
		self.testcaseParser = TestCaseResultParser()
		self.lineNumber = 0

	def parseResultLine(self,line):
		result = None
		if self.state == self.NONE:
			if line.startswith("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"):
				self.state = self.VERSION_EXPECTED
			elif len(line):
				self.current.output.append(line.strip())
				if len(self.current.output) == 10:
					if (self.current.output[0] == "Message from section one" and self.current.output[1] == "Message from section two" and
						self.current.output[2] == "Some information" and self.current.output[3] == "An error" and
						self.current.output[4] == "Message from section one" and self.current.output[5] == "Message from section two" and
						self.current.output[6] == "Some information" and self.current.output[7] == "An error" and
						self.current.output[8] == "hello" and self.current.output[9] == "hello" ):

						self.current.outputLine = self.lineNumber - 9

		elif self.state == self.VERSION_EXPECTED:
			m = self.versionParser.match(line)
			if m:
				self.current.appname = m.group(1).strip()
				self.current.version = m.group(2).strip()
				self.state = self.TEST_CASE_EXPECTED
			elif len(line):
				raise Exception("Unknown parse line: '" + line + "'")
		elif self.state == self.TEST_CASE_EXPECTED:
			if line == "Run with -? for options":
				pass
			else:
				testcase = None
				try:
					testcase = self.testcaseParser.parseResultLine(line)
				except RandomOutput as e:
					#print "E:", self.lineNumber, ", ",e.output
					self.current.output = e.output
					self.current.outputLine = self.lineNumber - 10
				
				if isinstance(testcase, TestCaseData):
					self.current.testcases.append(testcase)
					if line.startswith("==============================================================================="):
						self.state = self.END_RUN_INFO
		elif self.state == self.END_RUN_INFO:
			if len(line):
				self.current.results = line.strip()
				result = self.current

		self.lineNumber += 1
		return result

class TestRunResultHandler:

	def __init__(self, filePath):
		rawFile = open( filePath, 'r' )
		parser = TestRunResultParser()
		lineNumber = 0
		self.current = None
		for line in rawFile:
			line = line.rstrip()
			#print "L:", lineNumber, "'",line,"'"
			result = parser.parseResultLine(line)
			if isinstance(result, TestRunData):
				self.current = result
			lineNumber += 1
		if not(isinstance(self.current, TestRunData) ):
			raise Exception("File could not be parsed: '" + filePath + "'")

	def writeRawFile(self,filePath):
		rawWriteFile = open( filePath, 'wb' )
		lines = self.current.generateResultLines()
		for line in lines:
			rawWriteFile.write(line + os.linesep)

	def writeUnapprovedFile(self,filePath,outputLine):
		rawWriteFile = open( filePath, 'wb' )
		lines = self.current.generateUnapprovedLines(outputLine)
		for line in lines:
			rawWriteFile.write(line + "\n")
	def writeSortedUnapprovedFile(self,filePath,outputLine):
		rawWriteFile = open( filePath, 'wb' )
		lines = self.current.generateSortedUnapprovedLines(outputLine)
		for line in lines:
			rawWriteFile.write(line + "\n")

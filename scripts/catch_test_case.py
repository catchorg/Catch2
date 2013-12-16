import re

from catch_conditions import TestConditionData
from catch_conditions import TestConditionApprovedParser
from catch_conditions import TestConditionResultParser

class EndOfClassName:
	@staticmethod
	def parseRawLine(line):
		if line.startswith("..............................................................................."):
			return ParseResult.END_OF_CLASS_NAME
		return ParseResult.NONE

class TestCaseData:

	def __init__(self):
		self.name = ""
		self.nameParts = []
		self.classname = "global"
		self.sections = []
		self.filenamePrefix = ""
		self.filename = ""
		self.lineNumber = ""
		self.conditions = []
		self.sysout = []
		self.syserr = []
		self.result = ""

	def __eq__(self, other):
		return self.__dict__ == other.__dict__

	def __repr__(self):
		result = "[" + self.name + ", [ "
		suffix = ""
		for section in self.sections:
			result += suffix
			result += repr(section)
			suffix = ", "
		result + " ] "
		result += self.filename + ", " + self.lineNumber + " [ "
		suffix = ""
		for cond in self.conditions:
			result += suffix
			result += repr(cond)
			suffix = ", "
		result + " ] ]"
		return result

	def empty(self):
		if len(self.name):
			return False
		return True

	def generateApprovedLines(self):
		if self.empty():
			raise Exception("Empty test case..." + repr(self))
		lines = []
		if len(self.name):
			lines.append("-------------------------------------------------------------------------------")
			for n in self.nameParts:
				lines.append(n)
			if len(self.sections) > 0:
				for section in self.sections:
					lines.append("  " + section)
			lines.append("-------------------------------------------------------------------------------")
		if len(self.filename):
			lines.append(self.filename + ":<" + self.lineNumber + ">")
			lines.append("...............................................................................")
		lines.append("")
		for cond in self.conditions:
			lines += cond.generateApprovedLines()
		return lines

	def generateResultLines(self):
		if self.empty():
			raise Exception("Empty test case..." + repr(self))
		lines = []
		if len(self.name):
			lines.append("-------------------------------------------------------------------------------")
			for n in self.nameParts:
				lines.append(n)
			if len(self.sections) > 0:
				for section in self.sections:
					lines.append("  " + section)
			lines.append("-------------------------------------------------------------------------------")
		if len(self.filename):
			lines.append(self.filenamePrefix + self.filename + "(" + self.lineNumber + ")")
			lines.append("...............................................................................")
		lines.append("")
		for cond in self.conditions:
			lines += cond.generateResultLines()
		return lines

	def generateUnapprovedLines(self):
		if self.empty():
			raise Exception("Empty test case..." + repr(self))
		lines = []
		if len(self.name):
			lines.append("-------------------------------------------------------------------------------")
			for n in self.nameParts:
				lines.append(n)
			if len(self.sections) > 0:
				for section in self.sections:
					lines.append("  " + section)
			lines.append("-------------------------------------------------------------------------------")
		if len(self.filename):
			lines.append(self.filename + ":<" + "line number" + ">")
			lines.append("...............................................................................")
		lines.append("")
		for cond in self.conditions:
			lines += cond.generateUnapprovedLines()
		return lines
	
	def generateUnapprovedJunit(self):
		lines = []

		condLines = []
		for cond in self.conditions:
			condLines += cond.generateUnapprovedJunit()

		if len(self.name):
			l = "    <testcase classname=\"" + self.classname + "\" name=\"" + self.name + "\" time=\"{duration}\""
			if len(condLines) > 0 or len(self.sysout) > 0 or len(self.syserr) > 0:
				l += ">"
			else:
				l += "/>"
			lines.append(l)
			#if len(self.sections) > 0:
			#	for section in self.sections:
			#		lines.append("  " + section)
		#if len(self.filename):
		#	lines.append(self.filename + ":<" + "line number" + ">")
		#	lines.append("...............................................................................")
		lines += condLines
		if len(self.sysout) > 0:
			lines.append("      <system-out>")
			for l in self.sysout:
				lines.append(l)
			lines.append("      </system-out>")
		if len(self.syserr) > 0:
			lines.append("      <system-err>")
			for l in self.syserr:
				lines.append(l)
			lines.append("      </system-err>")
		if len(condLines) > 0 or len(self.sysout) > 0 or len(self.syserr) > 0:
			lines.append("    </testcase>")
		return lines

	def generateRecursiveSection(self, prefix, section):
		lines = []
		#print "S:",section
		if section[0] == "S":
			l = "    " + prefix + "<Section name=\"" + section[1] + "\""
			if section[2] != None:
				li = section[2]
				li = li.replace("&","&amp;")
				li = li.replace("<","&lt;")
				li = li.replace("&lt;hex digits>","<hex digits>")
				li = li.replace("\"","&quot;")
				l += " description=\"" + li + "\""
			l += ">"
			lines.append(l)
			if len(section) > 4:
				index = 4
				while index < len(section):
					tmp = section[index]
					if len(tmp) > 0:
						if tmp[0] == "E":
							l = "      " + prefix + "<Expression success=\"" + tmp[1] + "\""
							if tmp[2] != None:
								l += " filename=\"" + tmp[2] + "\""
							l += " >"
							lines.append(l)
							if len(tmp) > 3:
								cond = tmp[3]
								if len(cond[0]) > 0:
									lines.append("        " + prefix + "<Original>")
									for li in cond[0]:
										if len(li.strip()) > 0:
											li = li.replace("<","&lt;")
											li = li.replace("&lt;hex digits>","<hex digits>")
											li = li.replace("\"","&quot;")
											lines.append(li)
									lines.append("        " + prefix + "</Original>")
								if len(cond[1]) > 0:
									lines.append("        " + prefix + "<Expanded>")
									for li in cond[1]:
										if len(li.strip()) > 0:
											li = li.replace("<","&lt;")
											li = li.replace("&lt;hex digits>","<hex digits>")
											li = li.replace("\"","&quot;")
											lines.append(li)
									lines.append("        " + prefix + "</Expanded>")
								if len(cond) > 2:
									filename = cond[2]
									lines.append("      " + prefix + "<Exception filename=\"" + filename + "\" >")
									if len(cond) > 3:
										tmp = cond[3]
										for li in tmp:
											if len(li.strip()) > 0:
												lines.append(li)
									lines.append("        " + prefix + "</Exception>")
							elif len(tmp) > 4:
								print "RE:",tmp[4]

							l = "      " + prefix + "</Expression>"
							lines.append(l)
						elif tmp[0] == "X":
							l = "      " + prefix + "<Exception filename=\"" + tmp[1] + "\" >"
							lines.append(l)
							for li in tmp[2]:
								if len(li.strip()) > 0:
									lines.append(li)
							l = "      " + prefix + "</Exception>"
							lines.append(l)
						elif tmp[0] == "F":
							if len(tmp[1]) > 0:
								l = "      " + prefix + "<Failure>"
								lines.append(l)
								for li in tmp[1]:
									if len(li.strip()) > 0:
										lines.append(li)
								l = "      " + prefix + "</Failure>"
								lines.append(l)
							else:
								l = "    " + prefix + "<Failure/>"
								lines.append(l)
						elif tmp[0] == "S":
							lines += self.generateRecursiveSection(prefix + "  ", tmp)
						else:
							print "RS2:",tmp[0]
					else:
						print "RS:",section[index]
					index += 1

			lines.append("      " + prefix + "<OverallResults successes=\"" + section[3][0] + "\" failures=\"" + section[3][1] + "\"/>")
			l = "    " + prefix + "</Section>"
			lines.append(l)
		return lines

	def generateSection(self, prefix, sections):
		lines = []
		for section in sections:
			#print "S:",section
			if section[0] == "S":
				lines += self.generateRecursiveSection(prefix, section)
			elif section[0] == "E":
				l = "    " + prefix + "<Expression success=\"" + section[1] + "\""
				if section[2] != None:
					l += " filename=\"" + section[2] + "\""
				l += " >"
				lines.append(l)
				if len(section) > 3:
					cond = section[3]
					if len(cond[0]) > 0:
						lines.append("      " + prefix + "<Original>")
						for li in cond[0]:
							if len(li.strip()) > 0:
								li = li.replace("&","&amp;")
								li = li.replace("<","&lt;")
								li = li.replace("&lt;hex digits>","<hex digits>")
								li = li.replace("\"","&quot;")
								lines.append(li)
						lines.append("      " + prefix + "</Original>")
					if len(cond[1]) > 0:
						lines.append("      " + prefix + "<Expanded>")
						for li in cond[1]:
							if len(li.strip()) > 0:
								li = li.replace("<","&lt;")
								li = li.replace("&lt;hex digits>","<hex digits>")
								li = li.replace("\"","&quot;")
								lines.append(li)
						lines.append("      " + prefix + "</Expanded>")
					if len(cond) > 2:
						filename = cond[2]
						lines.append("      " + prefix + "<Exception filename=\"" + filename + "\" >")
						if len(cond) > 3:
							tmp = cond[3]
							for li in tmp:
								if len(li.strip()) > 0:
									lines.append(li)
						lines.append("      " + prefix + "</Exception>")
				elif len(section) > 4:
					print "RE:",section[4]

				l = "    " + prefix + "</Expression>"
				lines.append(l)
			elif section[0] == "X":
				l = "    " + prefix + "<Exception filename=\"" + section[1] + "\" >"
				lines.append(l)
				for li in section[2]:
					if len(li.strip()) > 0:
						lines.append(li)
				l = "    " + prefix + "</Exception>"
				lines.append(l)
			elif section[0] == "I":
				l = "    " + prefix + "<Info>"
				lines.append(l)
				for li in section[1]:
					if len(li.strip()) > 0:
						lines.append(li)
				l = "    " + prefix + "</Info>"
				lines.append(l)
			elif section[0] == "W":
				l = "    " + prefix + "<Warning>"
				lines.append(l)
				for li in section[1]:
					if len(li.strip()) > 0:
						lines.append(li)
				l = "    " + prefix + "</Warning>"
				lines.append(l)
			elif section[0] == "F":
				if len(section[1]) > 0:
					l = "    " + prefix + "<Failure>"
					lines.append(l)
					for li in section[1]:
						if len(li.strip()) > 0:
							lines.append(li)
					l = "    " + prefix + "</Failure>"
					lines.append(l)
				else:
					l = "    " + prefix + "<Failure/>"
					lines.append(l)
		return lines

	def generateUnapprovedXml(self):
		lines = []

		if len(self.name):
			l = "    <TestCase name=\"" + self.name + "\">"
			lines.append(l)

		if len(self.sections) > 0:
			prefix = "  "
			lines += self.generateSection(prefix, self.sections)

		if len(self.result) > 0:
			lines.append("      <OverallResult success=\"" + self.result + "\"/>")

		if len(self.name):
			l = "    </TestCase>"
			lines.append(l)
		return lines

	def addFailure(self, filename, lineNumber, output, message, type):
		self.filename = filename
		self.lineNumber = lineNumber
		condition = TestConditionData()
		condition.addFailure(filename, lineNumber, output, message, type)
		self.conditions.append(condition)

	def addError(self, filename, lineNumber, output, message, type):
		self.filename = filename
		self.lineNumber = lineNumber
		condition = TestConditionData()
		condition.addError(filename, lineNumber, output, message, type)
		self.conditions.append(condition)

	def addSysout(self, output):
		self.sysout = output

	def addSyserr(self, output):
		self.syserr = output

	def addOverallResult(self,r):
		self.result = r

	def addSection(self,name,desc, results):
		section = []
		section.append("S")
		section.append(name)
		section.append(desc)
		section.append(results)
		self.sections.append(section)
		return section

	def addExpression(self,result,filename):
		section = []
		section.append("E")
		section.append(result)
		section.append(filename)
		self.sections.append(section)
		return section

	def addException(self,filename,text):
		section = []
		section.append("X")
		section.append(filename)
		section.append(text)
		#print section
		self.sections.append(section)
		return section

	def addInfo(self,text):
		section = []
		section.append("I")
		section.append(text)
		self.sections.append(section)
		return section

	def addWarning(self,text):
		section = []
		section.append("W")
		section.append(text)
		self.sections.append(section)
		return section

	def addSimpleFailure(self,text):
		section = []
		section.append("F")
		section.append(text)
		self.sections.append(section)
		return section

	def addExpressionDetails(self, section, subExp):
		section.append(subExp)

	def addSubException(self, section, filename, text):
		tmp = []
		tmp.append("X")
		tmp.append(filename)
		tmp.append(text)
		section.append(tmp)

	def addSubFailure(self, section, text):
		tmp = []
		tmp.append("F")
		tmp.append(text)
		section.append(tmp)

	def addSubExpression(self,section,result,filename):
		tmp = []
		tmp.append("E")
		tmp.append(result)
		tmp.append(filename)
		section.append(tmp)
		#print "Section:",section
		return tmp

	def addSubSection(self,section,name,desc,results):
		tmp = []
		tmp.append("S")
		tmp.append(name)
		tmp.append(desc)
		tmp.append(results)
		section.append(tmp)
		#print "Section:",section
		return tmp

class TestCaseApprovedParser:
	NONE = 0
	TEST_CASE_NAME_EXPECTED = 1
	TEST_CASE_NAME = 2
	TEST_CLASS_EXPECTED = 3
	END_OF_CLASS_NAME_EXPECTED = 4
	TEST_CONDITION_EXPECTED = 5

	testFilenameParser = re.compile( r'(.*\..pp).*:<(.*).*>' )
	
	def __init__(self):
		self.state = self.NONE
		self.current = TestCaseData()
		self.conditionParser = TestConditionApprovedParser()

	def parseApprovedLine(self,line):
		result = None
		if self.state == self.NONE:
			if line.startswith("-------------------------------------------------------------------------------"):
				self.state = self.TEST_CASE_NAME_EXPECTED
			elif len(line):
				raise Exception("Unknown parse line: '" + line + "'")
		elif self.state == self.TEST_CASE_NAME_EXPECTED:
			if len(line):
				self.current.name = line.strip()
				self.current.nameParts.append(line.strip())
				self.state = self.TEST_CASE_NAME
			elif len(line):
				raise Exception("Unknown parse line: '" + line + "'")
		elif self.state == self.TEST_CASE_NAME:
			if line.startswith("-------------------------------------------------------------------------------"):
				self.state = self.TEST_CLASS_EXPECTED
			elif line.startswith("  "):
				#print "***SECTION: ",line
				self.current.sections.append(line[2:])
			elif len(line):
				if len(self.current.name) > 0:
					self.current.name += line.strip()
				else:
					self.current.name = line.strip()
				self.current.nameParts.append(line.strip())
		elif self.state == self.TEST_CLASS_EXPECTED:
			m = self.testFilenameParser.match(line)
			if m:
				self.current.filename = m.group(1).strip()
				self.current.lineNumber = m.group(2).strip()
				self.state = self.END_OF_CLASS_NAME_EXPECTED
			elif len(line):
				raise Exception("Unknown parse line: '" + line + "'")
		elif self.state == self.END_OF_CLASS_NAME_EXPECTED:
			if line.startswith("..............................................................................."):
				self.state = self.TEST_CONDITION_EXPECTED
			elif len(line):
				raise Exception("Unknown parse line: '" + line + "'")
		elif self.state == self.TEST_CONDITION_EXPECTED:
			#print "**** LINE " + line
			condition = self.conditionParser.parseApprovedLine(line)
			if isinstance(condition, TestConditionData):
				#print "**** CASE " + repr(condition)
				self.current.conditions.append(condition)
				if line.startswith("-------------------------------------------------------------------------------"):
					result = self.current
					self.current = TestCaseData()
					self.state = self.TEST_CASE_NAME_EXPECTED
				elif line.startswith("==============================================================================="):
					result = self.current
					self.current = TestCaseData()
					self.state = self.NONE

		return result

class TestCaseResultParser:
	NONE = 0
	TEST_CASE_NAME_EXPECTED = 1
	TEST_CASE_NAME = 2
	TEST_CLASS_EXPECTED = 3
	END_OF_CLASS_NAME_EXPECTED = 4
	TEST_CONDITION_EXPECTED = 5

	testFilenameParser = re.compile( r'(.*\\)(.*\..pp).*\((.*).*\)' )
	
	def __init__(self):
		self.state = self.NONE
		self.current = TestCaseData()
		self.conditionParser = TestConditionResultParser()

	def parseResultLine(self,line):
		result = None
		if self.state == self.NONE:
			if line.startswith("-------------------------------------------------------------------------------"):
				self.state = self.TEST_CASE_NAME_EXPECTED
			elif len(line):
				raise Exception("Unknown parse line: '" + line + "'")
		elif self.state == self.TEST_CASE_NAME_EXPECTED:
			if len(line):
				self.current.name = line.strip()
				self.current.nameParts.append(line.strip())
				self.state = self.TEST_CASE_NAME
			elif len(line):
				raise Exception("Unknown parse line: '" + line + "'")
		elif self.state == self.TEST_CASE_NAME:
			if line.startswith("-------------------------------------------------------------------------------"):
				self.state = self.TEST_CLASS_EXPECTED
			elif line.startswith("  "):
				#print "***SECTION: ",line
				self.current.sections.append(line[2:])
			elif len(line):
				if len(self.current.name) > 0:
					self.current.name += line.strip()
				else:
					self.current.name = line.strip()
				self.current.nameParts.append(line.strip())
		elif self.state == self.TEST_CLASS_EXPECTED:
			m = self.testFilenameParser.match(line)
			if m:
				self.current.filenamePrefix = m.group(1).strip()
				self.current.filename = m.group(2).strip()
				self.current.lineNumber = m.group(3).strip()
				self.state = self.END_OF_CLASS_NAME_EXPECTED
			elif len(line):
				raise Exception("Unknown parse line: '" + line + "'")
		elif self.state == self.END_OF_CLASS_NAME_EXPECTED:
			if line.startswith("..............................................................................."):
				self.state = self.TEST_CONDITION_EXPECTED
			elif len(line):
				raise Exception("Unknown parse line: '" + line + "'")
		elif self.state == self.TEST_CONDITION_EXPECTED:
			#print "**** LINE " + line
			condition = self.conditionParser.parseResultLine(line)
			if isinstance(condition, TestConditionData):
				#print "**** CASE " + repr(condition)
				self.current.conditions.append(condition)
				if line.startswith("-------------------------------------------------------------------------------"):
					result = self.current
					self.current = TestCaseData()
					self.state = self.TEST_CASE_NAME_EXPECTED
				elif line.startswith("==============================================================================="):
					result = self.current
					self.current = TestCaseData()
					self.state = self.NONE

		return result


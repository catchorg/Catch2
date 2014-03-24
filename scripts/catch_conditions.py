import re

class RandomOutput:

	def __init__(self):
		output = []

class TestConditionData:
	NONE = 0
	CONDITION = 1
	EXPANSION = 2
	MESSAGES = 3
	
	hexParser = re.compile( r'(.*)\b(0[xX][0-9a-fA-F]+)\b(.*)' )

	def __init__(self):
		self.state = self.NONE
		self.filenamePrefix = ""
		self.filename = ""
		self.lineNumber = ""
		self.reason = ""
		self.condition = ""
		self.expansionPrefix = ""
		self.expansion = []
		self.messagePrefix = ""
		self.messages = []
		self.output = []
		self.noAssertions = ""
	
	def __eq__(self, other): 
		return (self.filenamePrefix == other.filenamePrefix and
				self.filename == other.filename and self.lineNumber == other.lineNumber and
				self.reason == other.reason and self.condition == other.condition and
				self.expansion == other.expansion and self.messagePrefix == other.messagePrefix and
				self.output == other.output and self.noAssertions == other.noAssertions)


	def empty(self):
		if self.state == self.NONE:
			return True
		return False

	def __repr__(self):
		result = ("[" + self.reason + ", " + self.filename + ", " + self.lineNumber + ", " + self.condition +
					", " + self.expansionPrefix + ", [ ")
		suffix = ""
		for msg in self.expansion:
			result += suffix
			result += repr(msg)
			suffix = ", "
		result += "], " + self.messagePrefix + ", [ "
		suffix = ""
		for msg in self.messages:
			result += suffix
			result += repr(msg)
			suffix = ", "
		result += " ], [ "
		suffix = ""
		for msg in self.output:
			result += suffix
			result += repr(msg)
			suffix = ", "
		result += " ] ]"
		return result

	def parseCondition(self,line):
		if len(line):
			if self.state == self.CONDITION and line.startswith("  "):
				self.condition = line.strip()
			elif self.state == self.CONDITION:
				if len(self.reason) == 0 and line.startswith("PASSED:"):
					self.reason = line.strip()
				elif line.startswith("warning:") or line.startswith("with message"):
					self.messagePrefix = line.strip()
					self.state = self.MESSAGES
				elif not(line.startswith("  ")):
					self.expansionPrefix = line.strip()
					self.state = self.EXPANSION
				else:
					raise Exception("Unknown condition parse line: '" + line + "'")
			elif self.state == self.EXPANSION:
				if line.startswith("with message"):
					self.messagePrefix = line.strip()
					self.state = self.MESSAGES
				elif line.startswith("  "):
					#print "***LINE: ", line
					self.expansion.append(line[2:].rstrip())
				elif line.startswith("... message truncated due"):
					#print "***MSG: ", line
					self.messagePrefix = line.strip()
				else:
					#print "***OUTPUT: ", line
					self.output.append(line.strip())
			elif self.state == self.MESSAGES:
				if line.startswith("  "):
					#print "***MESSAGE: ", line
					self.messages.append(line.strip())
				else:
					#print "***OUTPUT: ", line
					self.output.append(line.strip())
			else:
				raise Exception("Unknown parse line: '" + line + "'")

			if len(self.output) == 10:
				if (self.output[0] == "Message from section one" and self.output[1] == "Message from section two" and
					self.output[2] == "Some information" and self.output[3] == "An error" and
					self.output[4] == "Message from section one" and self.output[5] == "Message from section two" and
					self.output[6] == "Some information" and self.output[7] == "An error" and
					self.output[8] == "hello" and self.output[9] == "hello" ):
					obj = RandomOutput()
					obj.output = self.output
					self.output = []
					raise obj

			elif len(self.output) == 2:
				if (self.output[0] == "hello" and self.output[1] == "hello" ):
					obj = RandomOutput()
					obj.output = self.output
					self.output = []
					raise obj

	def generateApprovedLines(self):
		if self.empty():
			raise Exception("Empty condition..." + repr(self))
		lines = []
		extraLine = False
		if len(self.filename):
			line = self.filename + ":<" + self.lineNumber + ">:"
			reasonOnSameLine = False
			if self.reason == "FAILED":
				line += " " + "FAILED:"
				reasonOnSameLine = True
			lines.append(line)
			if not(reasonOnSameLine) and len(self.reason):
				lines.append(self.reason)
		if len(self.condition):
			line = self.condition
			m = self.hexParser.match(line)
			if m:
				while m:
					line = m.group(1) + "0x<hex digits>" + m.group(3)
					m = self.hexParser.match(line)
			line = line.replace("__null","0")
			lines.append("  " + line)
		if len(self.expansionPrefix):
			lines.append(self.expansionPrefix)
			extraLine = True
		if len(self.expansion):
			for line in self.expansion:
				m = self.hexParser.match(line)
				if m:
					while m:
						line = m.group(1) + "0x<hex digits>" + m.group(3)
						m = self.hexParser.match(line)
				line = line.replace("__null","0")
				lines.append("  " + line)
		if len(self.messagePrefix):
			lines.append(self.messagePrefix)
			extraLine = True
		if len(self.messages):
			for msg in self.messages:
				lines.append("  " + msg)
		lines.append("")
		if len(self.noAssertions) > 0:
			if extraLine:
				lines.append("")
			lines.append(self.noAssertions)
			lines.append("")
		if len(self.output):
			for msg in self.output:
				lines.append(msg)
		return lines
		
	def generateResultLines(self):
		if self.empty():
			raise Exception("Empty condition..." + repr(self))
		lines = []
		extraLine = False
		if len(self.filename):
			if len(self.filenamePrefix):
				line = self.filenamePrefix + self.filename + "(" + self.lineNumber + "):"
			else:
				line = self.filename + "(" + self.lineNumber + "):"
			reasonOnSameLine = False
			if self.reason == "FAILED":
				line += " " + "FAILED:"
				reasonOnSameLine = True
			if (len(self.messagePrefix) > 0 and self.messagePrefix == "warning:" or
					self.reason == "PASSED:" or self.expansionPrefix.startswith("FAILED - but was ok") ):
				line += " "
			lines.append(line)
			if not(reasonOnSameLine) and len(self.reason):
				lines.append(self.reason)
		if len(self.condition):
			lines.append("  " + self.condition)
		if len(self.expansionPrefix):
			lines.append(self.expansionPrefix)
			extraLine = True
		if len(self.expansion):
			for line in self.expansion:
				lines.append("  " + line)
		if len(self.messagePrefix):
			lines.append(self.messagePrefix)
			extraLine = True
		if len(self.messages):
			for msg in self.messages:
				lines.append("  " + msg)
		lines.append("")
		if len(self.noAssertions) > 0:
			if extraLine:
				lines.append("")
			lines.append(self.noAssertions)
			lines.append("")
		if len(self.output):
			for msg in self.output:
				lines.append(msg)
		return lines
		
	def generateUnapprovedLines(self):
		if self.empty():
			raise Exception("Empty condition..." + repr(self))
		lines = []
		extraLine = False
		if len(self.filename):
			line = self.filename + ":<" + "line number" + ">:"
			reasonOnSameLine = False
			if self.reason == "FAILED":
				line += " " + "FAILED:"
				reasonOnSameLine = True
			lines.append(line)
			if not(reasonOnSameLine) and len(self.reason):
				lines.append(self.reason)
		if len(self.condition):
			line = self.condition
			m = self.hexParser.match(line)
			if m:
				while m:
					line = m.group(1) + "0x<hex digits>" + m.group(3)
					m = self.hexParser.match(line)
			line = line.replace("Catch::Text","Text")
			lines.append("  " + line)
		if len(self.expansionPrefix):
			lines.append(self.expansionPrefix)
			extraLine = True
		if len(self.expansion):
			for line in self.expansion:
				m = self.hexParser.match(line)
				if m:
					while m:
						line = m.group(1) + "0x<hex digits>" + m.group(3)
						m = self.hexParser.match(line)
				lines.append("  " + line)
		if len(self.messagePrefix):
			lines.append(self.messagePrefix)
			extraLine = True
		if len(self.messages):
			for msg in self.messages:
				lines.append("  " + msg)
		lines.append("")
		if len(self.noAssertions) > 0:
			if extraLine:
				lines.append("")
			lines.append(self.noAssertions)
			lines.append("")
		if len(self.output):
			for msg in self.output:
				lines.append(msg)
		return lines
		
	def addFailure(self, filename, lineNumber, output, message, type):
		self.reason = "failure"
		self.filename = filename
		self.lineNumber = lineNumber
		self.condition = type
		if message != None:
			self.expansion.append(message)
		self.output = output

	def addError(self, filename, lineNumber, output, message, type):
		self.reason = "error"
		self.filename = filename
		self.lineNumber = lineNumber
		self.condition = type
		if message != None:
			self.expansion.append(message)
		self.output = output

	def generateUnapprovedJunit(self):
		lines = []
		msg = ""
		for m in self.expansion:
			msg += m
		msg = msg.replace("\"", "&quot;")
		msg = msg.replace("<", "&lt;")
		msg = msg.replace("&lt;hex digits>", "<hex digits>")
		#msg = msg.replace(">", "&gt;")
		
		#print "R:",self.reason,msg,self.condition
		if len(self.reason) > 0:
			l = "      <" + self.reason
			if len(msg) > 0:
				m = self.hexParser.match(msg)
				if m:
					while m:
						msg = m.group(1) + "0x<hex digits>" + m.group(3)
						m = self.hexParser.match(msg)
				l += " message=\"" + msg + "\""
			if self.condition != None:
				l += " type=\"" + self.condition + "\""
			l += ">"
			lines.append(l)
			if len(self.output) > 0:
				for o in self.output:
					lines.append(o)
			if len(self.filename) > 0:
				lines.append(self.filename + ":<" + self.lineNumber + ">")
			lines.append("      </" + self.reason + ">")
		return lines

class TestConditionApprovedParser:
	failedApprovedTestParser = re.compile( r'(.*\..pp).*:<(.*).*>:(.*FAILED)' )
	otherApprovedTestParser = re.compile( r'(.*\..pp).*:<(.*).*>:' )
	
	def __init__(self):
		self.current = TestConditionData()

	def parseApprovedLine(self,line):
		result = None
		if line.startswith("==============================================================================="):
			if not(self.current.empty()):
				result = self.current
				self.current = TestConditionData()
		
		elif line.startswith("-------------------------------------------------------------------------------"):
			if not(self.current.empty()):
				result = self.current
				self.current = TestConditionData()

		else:
			if line.startswith("No assertions in"):
				self.current.noAssertions = line.strip()
				self.current.state = self.current.MESSAGES
			else:
				m = self.failedApprovedTestParser.match(line)
				if m:
					if not(self.current.empty()):
						result = self.current
						self.current = TestConditionData()
					self.current.filename = m.group(1).strip()
					self.current.lineNumber = m.group(2).strip()
					self.current.reason = m.group(3).strip()
					self.current.state = self.current.CONDITION
				else:
					m = self.otherApprovedTestParser.match(line)
					if m:
						if not(self.current.empty()):
							result = self.current
							self.current = TestConditionData()
						self.current.filename = m.group(1).strip()
						self.current.lineNumber = m.group(2).strip()
						self.current.state = self.current.CONDITION
					elif not(self.current.empty()):
						self.current.parseCondition(line)
		return result

class TestConditionResultParser:
	failedResultsTestParser = re.compile( r'(.*\\)(.*\..pp).*\((.*).*\):(.*FAILED)' )
	otherResultsTestParser = re.compile( r'(.*\\)(.*\..pp).*\((.*).*\):' )
	
	def __init__(self):
		self.current = TestConditionData()

	def parseResultLine(self,line):
		result = None
		if line.startswith("==============================================================================="):
			if not(self.current.empty()):
				result = self.current
				self.current = TestConditionData()
		
		elif line.startswith("-------------------------------------------------------------------------------"):
			if not(self.current.empty()):
				result = self.current
				self.current = TestConditionData()

		else:
			if line.startswith("No assertions in"):
				self.current.noAssertions = line.strip()
				self.current.state = self.current.MESSAGES
			else:
				m = self.failedResultsTestParser.match(line)
				if m:
					if not(self.current.empty()):
						result = self.current
						self.current = TestConditionData()
					self.current.filenamePrefix = m.group(1).strip()
					self.current.filename = m.group(2).strip()
					self.current.lineNumber = m.group(3).strip()
					self.current.reason = m.group(4).strip()
					self.current.state = self.current.CONDITION
				else:
					m = self.otherResultsTestParser.match(line)
					if m:
						if not(self.current.empty()):
							result = self.current
							self.current = TestConditionData()
						self.current.filenamePrefix = m.group(1).strip()
						self.current.filename = m.group(2).strip()
						self.current.lineNumber = m.group(3).strip()
						self.current.state = self.current.CONDITION
					elif not(self.current.empty()):
						self.current.parseCondition(line)
		return result


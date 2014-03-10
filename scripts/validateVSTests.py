#!/c/Python27/python

import os
import sys
import subprocess
import re

import xml.etree.cElementTree as etree

from scriptCommon import catchPath
from catch_test_run  import TestRunApprovedHandler
from catch_test_run  import TestRunData
from catch_test_run  import TestRunResultHandler
from catch_test_case  import TestCaseResultParser
from catch_test_case  import TestCaseData
from catch_conditions import RandomOutput

rootPath = os.path.join(os.path.join(os.path.join( catchPath, 'projects'), 'SelfTest'), 'Baselines' )

if len(sys.argv) == 2:
	cmdPath = sys.argv[1]
else:
	if sys.platform == 'win32':
		cmdPath = os.path.join( catchPath, 'projects\\VS2010\\TestCatch\\Release\\TestCatch.exe' )
		# VS2010
		dllPath = os.path.join( catchPath, 'projects\\VS2010\\ManagedTestCatch\\Release\\ManagedTestCatch.dll' )
		#dllPath = os.path.join( catchPath, 'projects\\VS2010\\ManagedTestCatch\\Debug\\ManagedTestCatch.dll' )
		# VS2012 managed
		#dllPath = os.path.join( catchPath, 'projects\\VS2012\\ManagedTestCatch\\Debug\\ManagedTestCatch.dll' )
		# VS2012 native
		#dllPath = os.path.join( catchPath, 'projects\\VS2012\\NativeTestCatch\\Debug\\NativeTestCatch.dll' )
	else:
		cmdPath = os.path.join( catchPath, 'projects/XCode4/CatchSelfTest/DerivedData/CatchSelfTest/Build/Products/Debug/CatchSelfTest' )

print( cmdPath )

overallResult = 0

def approve( baseName, args ):
	global overallResult
	args[0:0] = [cmdPath]
	baselinesPath = os.path.join( rootPath, '{0}.approved.txt'.format( baseName ) )
	baselinesSortedPath = os.path.join( rootPath, '{0}.sorted.approved.txt'.format( baseName ) )
	rawResultsPath = os.path.join( rootPath, '_{0}.tmp'.format( baseName ) )
	if os.path.exists( baselinesPath ):
		approvedFileHandler = TestRunApprovedHandler(baselinesPath)
		#baselinesPathNew = os.path.join( rootPath, '{0}.approved.new.txt'.format( baseName ) )
		#approvedFileHandler.writeRawFile(baselinesPathNew)
		approvedFileHandler.writeSortedRawFile(baselinesSortedPath)
	else:
		raise Exception("Base file does not exist: '" + baselinesPath + "'")

	if not(os.path.exists( args[0] )):
		raise Exception("Executable does not exist: '" + args[0] + "'")

	f = open( rawResultsPath, 'w' )
	subprocess.call( args, stdout=f, stderr=f )
	f.close()

	if os.path.exists( rawResultsPath ):
		resultFileHandler = TestRunResultHandler(rawResultsPath)
		#rawPathNew = os.path.join( rootPath, '{0}.rewrite.txt'.format( baseName ) )
		#print( "F:" + rawPathNew," + ",approvedFileHandler.current.outputLine )
		#resultFileHandler.writeRawFile(rawPathNew)
		rawPathNewSorted = os.path.join( rootPath, '{0}.sorted.unapproved.txt'.format( baseName ) )
		resultFileHandler.writeSortedUnapprovedFile(rawPathNewSorted, approvedFileHandler.current.outputLine)
		os.remove( rawResultsPath )
	else:
		raise Exception("Results file does not exist: '" + rawResultsPath + "'")

def callDiff():
	#os.remove( rawResultsPath )
	print()
	print( baseName + ":" )
	if os.path.exists( baselinesSortedPath ) and os.path.exists( rawPathNewSorted ):
		diffResult = subprocess.call([ "diff", "--ignore-all-space", baselinesSortedPath, rawPathNewSorted ] )
		if diffResult == 0:
			#os.remove( filteredResultsPath )
			if not(sys.platform == 'win32'):
				print( "  \033[92mResults matched" )
			else:
				print( "  Results matched" )
		else:
			if not(sys.platform == 'win32'):
				print( "  \n****************************\n  \033[91mResults differed" )
			else:
				print( "  \n****************************\n  Results differed" )
			if diffResult > overallResult:
				overallResult = diffResult
		if not(sys.platform == 'win32'):
			print( "\033[0m" )

def approveJunit( baseName, args ):
	global overallResult
	args[0:0] = [cmdPath]
	baselinesPath = os.path.join( rootPath, '{0}.approved.txt'.format( baseName ) )
	baselinesSortedPath = os.path.join( rootPath, '{0}.sorted.approved.txt'.format( baseName ) )
	#baselinesFixedPath = os.path.join( rootPath, '{0}.rewrite.approved.txt'.format( baseName ) )
	rawResultsPath = os.path.join( rootPath, '_{0}.tmp'.format( baseName ) )
	if os.path.exists( baselinesPath ):
		xml = ""
		f = open( baselinesPath, 'r' )
		for line in f:
			xml += line
		xml = xml.replace("<line number>", "&lt;line number&gt;")
		xml = xml.replace("<hex digits>", "&lt;hex digits&gt;")
		#f2 = open( baselinesFixedPath, 'wb' )
		#f2.write(xml)
		#f2.close()
	
		# ClassTests.cpp:<line number>	
		otherApprovedTestParser = re.compile( r'(.*\..pp).*:<(.*).*>' )
		testRun = TestRunData()
		testcase = None
		root = etree.fromstring(xml)
		for testsuites in root:
			if testsuites.tag == "testsuite":
				testRun = TestRunData()
				testRun.appname = testsuites.get("name")
				testRun.errors = testsuites.get("errors")
				testRun.failures = testsuites.get("failures")
				testRun.tests = testsuites.get("tests")
				for tc in testsuites:
					if tc.tag == "testcase":
						cls = tc.get("classname")
						#print( "C:" + cls + tc )
						if len(cls):
							testcase = testRun.addClassTestCase(cls, tc.get("name"))
						else:
							testcase = testRun.addTestCase(tc.get("name"))
						for prop in tc:
							if prop.tag == "failure":
								text = prop.text.strip()
								lines = text.splitlines()
								filename = ""
								lineNumber = ""
								output = []
								for l in lines:
									m = otherApprovedTestParser.match(l)
									if m:
										filename = m.group(1)
										lineNumber = m.group(2)
									else:
										output.append(l)
								testcase.addFailure(filename, lineNumber, output, prop.get("message"), prop.get("type"))
							elif prop.tag == "error":
								text = prop.text.strip()
								lines = text.splitlines()
								filename = ""
								lineNumber = ""
								output = []
								for l in lines:
									m = otherApprovedTestParser.match(l)
									if m:
										filename = m.group(1)
										lineNumber = m.group(2)
									else:
										output.append(l)
								testcase.addError(filename, lineNumber, output, prop.get("message"), prop.get("type"))
							elif prop.tag == "system-out":
								text = prop.text.strip()
								lines = text.splitlines()
								testcase.addSysout(lines)
							elif prop.tag == "system-err":
								text = prop.text.strip()
								lines = text.splitlines()
								testcase.addSyserr(lines)
					elif tc.tag == "system-out":
						text = tc.text.strip()
						lines = text.splitlines()
						testRun.addSysout(lines)
					elif tc.tag == "system-err":
						text = tc.text.strip()
						lines = text.splitlines()
						testRun.addSyserr(lines)
					else:
						print( tc.tag )

		lines = testRun.generateSortedUnapprovedJunit()
		
		rawWriteFile = open( baselinesSortedPath, 'wb' )
		for line in lines:
			#print( "L:" + line )
			rawWriteFile.write(line + "\n")
		rawWriteFile.close()

	if not(os.path.exists( args[0] )):
		raise Exception("Executable does not exist: '" + args[0] + "'")

	f = open( rawResultsPath, 'w' )
	subprocess.call( args, stdout=f, stderr=f )
	f.close()

	rawSortedPath = os.path.join( rootPath, '{0}.sorted.unapproved.txt'.format( baseName ) )
	if os.path.exists( rawResultsPath ):
		xml = ""
		f = open( rawResultsPath, 'r' )
		for line in f:
			xml += line
		#xml = xml.replace("<line number>", "&lt;line number&gt;")
		#xml = xml.replace("<hex digits>", "&lt;hex digits&gt;")
	
		# ClassTests.cpp:<line number>	
		otherResultsTestParser = re.compile( r'(.*\\)(.*\..pp).*\((.*).*\)' )
		testRun = TestRunData()
		testcase = None
		root = etree.fromstring(xml)
		for testsuites in root:
			if testsuites.tag == "testsuite":
				testRun = TestRunData()
				testRun.appname = testsuites.get("name")
				testRun.errors = testsuites.get("errors")
				testRun.failures = testsuites.get("failures")
				testRun.tests = testsuites.get("tests")
				for tc in testsuites:
					if tc.tag == "testcase":
						cls = tc.get("classname")
						#print( "C:" + cls + tc )
						if len(cls):
							if cls.startswith("::"):
								cls = cls[2:]
							testcase = testRun.addClassTestCase(cls, tc.get("name"))
						else:
							testcase = testRun.addTestCase(tc.get("name"))
						for prop in tc:
							if prop.tag == "failure":
								text = prop.text.strip()
								lines = text.splitlines()
								filename = ""
								lineNumber = ""
								output = []
								for l in lines:
									m = otherResultsTestParser.match(l)
									if m:
										filename = m.group(2)
										lineNumber = "line number"
									else:
										output.append(l)
								testcase.addFailure(filename, lineNumber, output, prop.get("message"), prop.get("type"))
							elif prop.tag == "error":
								text = prop.text.strip()
								lines = text.splitlines()
								filename = ""
								lineNumber = ""
								output = []
								for l in lines:
									m = otherResultsTestParser.match(l)
									if m:
										filename = m.group(2)
										lineNumber = "line number"
									else:
										output.append(l)
								testcase.addError(filename, lineNumber, output, prop.get("message"), prop.get("type"))
							elif prop.tag == "system-out":
								text = prop.text.strip()
								lines = text.splitlines()
								testcase.addSysout(lines)
							elif prop.tag == "system-err":
								text = prop.text.strip()
								lines = text.splitlines()
								testcase.addSyserr(lines)
					elif tc.tag == "system-out":
						text = tc.text.strip()
						lines = text.splitlines()
						testRun.addSysout(lines)
					elif tc.tag == "system-err":
						text = tc.text.strip()
						lines = text.splitlines()
						testRun.addSyserr(lines)
					else:
						print( tc.tag )

		lines = testRun.generateSortedUnapprovedJunit()
		
		rawWriteFile = open( rawSortedPath, 'wb' )
		for line in lines:
			#print( "L:" + line )
			rawWriteFile.write(line + "\n")
		rawWriteFile.close()

def addSubSection(testcase, section, exp):
	r = exp.find("OverallResults")
	if r != None:
		ores = []
		ores.append(r.get("successes"))
		ores.append(r.get("failures"))
		if section == None:
			section = testcase.addSection(exp.get("name"), exp.get("description"), ores)
		else:
			section = testcase.addSubSection(section, exp.get("name"), exp.get("description"), ores)
		e1 = False
		for tmp in exp:
			if tmp.tag == "OverallResults":
				pass
			elif tmp.tag == "Exception":
				filename  = tmp.get("filename")
				text = tmp.text
				ls = text.splitlines()
				testcase.addSubException(section, filename, ls)
			elif tmp.tag == "Section":
				addSubSection(testcase, section, tmp)
			elif tmp.tag == "Failure":
				text = tmp.text
				ls = text.splitlines()
				testcase.addSubFailure(section, ls)
			elif tmp.tag == "Expression":
				#print( "Exp:" + tmp )
				e1 = True
				result = tmp.get("success")
				filename  = tmp.get("filename")
				subSection = testcase.addSubExpression(section,result,filename)
				subExp = []
				for cond in tmp:
					if cond.tag == "Original":
						text = cond.text
						ls = text.splitlines()
						subExp.append(ls)
					elif cond.tag == "Expanded" and len(subExp) == 1:
						text = cond.text
						ls = text.splitlines()
						subExp.append(ls)
					elif cond.tag == "Exception" and len(subExp) == 2:
						subExp.append(cond.get("filename"))
						text = cond.text
						ls = text.splitlines()
						subExp.append(ls)
					else:
						print( "SX:" + cond.tag )
				if len(subExp) >= 2:
					testcase.addExpressionDetails(subSection, subExp)
			else:
				print( "Z:" + tmp.tag )
		#if e1:
		#	print( "Section:" + section )

def addResultsSubSection(otherResultsTestParser, testcase, section, exp):
	r = exp.find("OverallResults")
	if r != None:
		ores = []
		ores.append(r.get("successes"))
		ores.append(r.get("failures"))
		if section == None:
			section = testcase.addSection(exp.get("name"), exp.get("description"), ores)
		else:
			section = testcase.addSubSection(section, exp.get("name"), exp.get("description"), ores)
		e1 = False
		for tmp in exp:
			if tmp.tag == "OverallResults":
				pass
			elif tmp.tag == "Exception":
				filename  = tmp.get("filename")
				m = otherResultsTestParser.match(filename)
				if m:
					filename = "/Users/philnash/Dev/OSS/Catch/projects/SelfTest/" + m.group(2)
				text = tmp.text
				ls = text.splitlines()
				testcase.addSubException(section, filename, ls)
			elif tmp.tag == "Section":
				addResultsSubSection(otherResultsTestParser, testcase, section, tmp)
			elif tmp.tag == "Failure":
				text = tmp.text
				ls = text.splitlines()
				testcase.addSubFailure(section, ls)
			elif tmp.tag == "Expression":
				#print( "Exp:" + tmp )
				e1 = True
				result = tmp.get("success")
				filename  = tmp.get("filename")
				m = otherResultsTestParser.match(filename)
				if m:
					filename = "/Users/philnash/Dev/OSS/Catch/projects/SelfTest/" + m.group(2)
				subSection = testcase.addSubExpression(section,result,filename)
				subExp = []
				for cond in tmp:
					if cond.tag == "Original":
						text = cond.text
						ls = text.splitlines()
						subExp.append(ls)
					elif cond.tag == "Expanded" and len(subExp) == 1:
						text = cond.text
						ls = text.splitlines()
						subExp.append(ls)
					elif cond.tag == "Exception" and len(subExp) == 2:
						filename = cond.get("filename")
						m = otherResultsTestParser.match(filename)
						if m:
							filename = "/Users/philnash/Dev/OSS/Catch/projects/SelfTest/" + m.group(2)
						subExp.append(filename)
						text = cond.text
						ls = text.splitlines()
						subExp.append(ls)
					else:
						print( "SX:" + cond.tag )
				if len(subExp) >= 2:
					testcase.addExpressionDetails(subSection, subExp)
			else:
				print( "Z:" + tmp.tag )
		#if e1:
		#	print( "Section:" + section )

def approveXml( baseName, args ):
	global overallResult
	args[0:0] = [cmdPath]
	baselinesPath = os.path.join( rootPath, '{0}.approved.txt'.format( baseName ) )
	baselinesSortedPath = os.path.join( rootPath, '{0}.sorted.approved.txt'.format( baseName ) )
	#baselinesFixedPath = os.path.join( rootPath, '{0}.rewrite.approved.txt'.format( baseName ) )
	rawResultsPath = os.path.join( rootPath, '_{0}.tmp'.format( baseName ) )
	if os.path.exists( baselinesPath ):
		xml = ""
		f = open( baselinesPath, 'r' )
		for line in f:
			xml += line
		xml = xml.replace("<hex digits>", "&lt;hex digits&gt;")

		#otherApprovedTestParser = re.compile( r'(.*\..pp).*:<(.*).*>' )
		testRun = TestRunData()
		testcase = None
		root = etree.fromstring(xml)
		testRun.appname = root.get("name")
		for ts in root:
			#print( ts.tag )
			for tc in ts:
				if tc.tag == "TestCase":
					testcase = testRun.addTestCase(tc.get("name"))
					for exp in tc:
						if exp.tag == "Expression":
							result = exp.get("success")
							filename  = exp.get("filename")
							section = testcase.addExpression(result,filename)
							subExp = []
							for cond in exp:
								if cond.tag == "Original":
									text = cond.text
									ls = text.splitlines()
									subExp.append(ls)
								elif cond.tag == "Expanded" and len(subExp) == 1:
									text = cond.text
									ls = text.splitlines()
									subExp.append(ls)
								elif cond.tag == "Exception" and len(subExp) == 2:
									subExp.append(cond.get("filename"))
									text = cond.text
									ls = text.splitlines()
									subExp.append(ls)
								else:
									print( "X:" + cond.tag )
							if len(subExp) >= 2:
								testcase.addExpressionDetails(section, subExp)
						elif exp.tag == "Exception":
							filename  = exp.get("filename")
							text = exp.text
							ls = text.splitlines()
							section = testcase.addException(filename,ls)
						elif exp.tag == "Section":
							addSubSection(testcase, None, exp)
						elif exp.tag == "Info":
							text = exp.text
							ls = text.splitlines()
							section = testcase.addInfo(ls)
						elif exp.tag == "Warning":
							text = exp.text
							ls = text.splitlines()
							section = testcase.addWarning(ls)
						elif exp.tag == "Failure":
							ls = []
							if exp.text != None:
								text = exp.text
								ls = text.splitlines()
							section = testcase.addSimpleFailure(ls)
						elif exp.tag == "OverallResult":
							testcase.addOverallResult(exp.get("success"))
						else:
							print( "E:" + exp.tag )
				elif tc.tag == "OverallResults":
					testRun.tests = tc.get("successes")
					testRun.failures = tc.get("failures")
				else:
					print( "U:" + tc.tag )

		lines = testRun.generateSortedUnapprovedXml()
		
		rawWriteFile = open( baselinesSortedPath, 'wb' )
		for line in lines:
			#print( "L:" + line )
			rawWriteFile.write(line + "\n")
		rawWriteFile.close()

	if not(os.path.exists( args[0] )):
		raise Exception("Executable does not exist: '" + args[0] + "'")

	f = open( rawResultsPath, 'w' )
	subprocess.call( args, stdout=f, stderr=f )
	f.close()

	rawSortedPath = os.path.join( rootPath, '{0}.sorted.unapproved.txt'.format( baseName ) )
	if os.path.exists( rawResultsPath ):
		xml = ""
		f = open( rawResultsPath, 'r' )
		for line in f:
			xml += line
		f.close()
		#xml = xml.replace("<hex digits>", "&lt;hex digits&gt;")
		os.remove( rawResultsPath )

		otherResultsTestParser = re.compile( r'(.*\\)(.*\..pp)' )
		hexParser = re.compile( r'(.*)\b(0[xX][0-9a-fA-F]+)\b(.*)' )
		testRun = TestRunData()
		testcase = None
		root = etree.fromstring(xml)
		testRun.appname = root.get("name")
		if testRun.appname == "TestCatch.exe":
			testRun.appname = "CatchSelfTest"
		for ts in root:
			#print( ts.tag )
			for tc in ts:
				if tc.tag == "TestCase":
					testcase = testRun.addTestCase(tc.get("name"))
					for exp in tc:
						if exp.tag == "Expression":
							result = exp.get("success")
							filename  = exp.get("filename")
							m = otherResultsTestParser.match(filename)
							if m:
								filename = "/Users/philnash/Dev/OSS/Catch/projects/SelfTest/" + m.group(2)
							section = testcase.addExpression(result,filename)
							subExp = []
							for cond in exp:
								if cond.tag == "Original":
									text = cond.text
									tmp = text.splitlines()
									ls = []
									for li in tmp:
										m = hexParser.match(li)
										if m:
											while m:
												#print( li + m.group(1) + m.group(3) )
												li = m.group(1) + "0x<hex digits>" + m.group(3)
												m = hexParser.match(li)
										ls.append(li)
									subExp.append(ls)
								elif cond.tag == "Expanded" and len(subExp) == 1:
									text = cond.text
									tmp = text.splitlines()
									ls = []
									for li in tmp:
										m = hexParser.match(li)
										if m:
											while m:
												#print( li + m.group(1) + m.group(3) )
												li = m.group(1) + "0x<hex digits>" + m.group(3)
												m = hexParser.match(li)
										ls.append(li)
									subExp.append(ls)
								elif cond.tag == "Exception" and len(subExp) == 2:
									filename = cond.get("filename")
									m = otherResultsTestParser.match(filename)
									if m:
										filename = "/Users/philnash/Dev/OSS/Catch/projects/SelfTest/" + m.group(2)
									subExp.append(filename)
									text = cond.text
									ls = text.splitlines()
									subExp.append(ls)
								else:
									print( "X:" + cond.tag )
							if len(subExp) >= 2:
								testcase.addExpressionDetails(section, subExp)
						elif exp.tag == "Exception":
							filename  = exp.get("filename")
							m = otherResultsTestParser.match(filename)
							if m:
								filename = "/Users/philnash/Dev/OSS/Catch/projects/SelfTest/" + m.group(2)
							text = exp.text
							ls = text.splitlines()
							section = testcase.addException(filename,ls)
						elif exp.tag == "Section":
							addResultsSubSection(otherResultsTestParser, testcase, None, exp)
						elif exp.tag == "Info":
							text = exp.text
							ls = text.splitlines()
							section = testcase.addInfo(ls)
						elif exp.tag == "Warning":
							text = exp.text
							ls = text.splitlines()
							section = testcase.addWarning(ls)
						elif exp.tag == "Failure":
							ls = []
							if exp.text != None:
								text = exp.text
								ls = text.splitlines()
							section = testcase.addSimpleFailure(ls)
						elif exp.tag == "OverallResult":
							testcase.addOverallResult(exp.get("success"))
						else:
							print( "E:" + exp.tag )
				elif tc.tag == "OverallResults":
					testRun.tests = tc.get("successes")
					testRun.failures = tc.get("failures")
				else:
					print( "U:" + tc.tag )

		lines = testRun.generateSortedUnapprovedXml()
		
		rawWriteFile = open( rawSortedPath, 'wb' )
		for line in lines:
			#print( "L:" + line )
			rawWriteFile.write(line + "\n")
		rawWriteFile.close()

def parseTrxFile(baseName, trxFile):
	print( "TRX file:" + trxFile )
	if os.path.exists( trxFile ):
		xml = ""
		f = open( trxFile, 'r' )
		for line in f:
			xml += line

		#otherResultsTestParser = re.compile( r'(.*\\)(.*\..pp)' )
		#hexParser = re.compile( r'(.*)\b(0[xX][0-9a-fA-F]+)\b(.*)' )
		testRun = TestRunData()
		testRun.appname = "CatchSelfTest"
		root = etree.fromstring(xml)
		if testRun.appname == "TestCatch.exe":
			testRun.appname = "CatchSelfTest"
		qname=re.compile("{(?P<ns>.*)}(?P<element>.*)")
		ids = []
		for ts in root:
			m = qname.match(ts.tag)
			if m:
				tag = m.group(2)
				print( tag )
			if tag != None:
				if tag == "TestDefinitions":
					for tc in ts:
						m = qname.match(tc.tag)
						if m:
							tag = m.group(2)
						if tag != None and tag == "UnitTest":
							name = tc.get("name")
							id = tc.get("id")
							for item in tc:
								m = qname.match(item.tag)
								if m:
									tag = m.group(2)
								if tag != None and tag == "Description":
									desc = item.text
									#print( desc + id )
									ids.append([id,desc])
				elif tag == "Results":
					#print( ids )
					ids = dict(ids)
					#print( ids["87ec526a-e414-1a3f-ba0f-e210b204bb42"] )
					
					lineNumber = 0
					resultParser = TestCaseResultParser()
					for tc in ts:
						m = qname.match(tc.tag)
						if m:
							tag = m.group(2)
						if tag != None and tag == "UnitTestResult":
							outcome = tc.get("outcome")
							id = tc.get("testId")
							if len(id) > 0:
								for item in tc:
									m = qname.match(item.tag)
									if m:
										tag = m.group(2)
									if tag != None and tag == "Output":
										for sub in item:
											m = qname.match(sub.tag)
											if m:
												tag = m.group(2)
											if tag != None and tag == "StdOut":
												desc = sub.text
												lines = desc.splitlines()
												found = False
												index = 0
												for tmp in lines:
													if (len(lines) >= (index + 2) and
															lines[index].startswith("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~") and
															lines[index + 1].startswith("Batch run using") ):
														found = True
														break
													index += 1
												lines = lines[index + 3:]
												#print( "*******" + desc )
												#print( lines )
												if found:
													endOfRun = False
													for line in lines:
														if endOfRun:
															testRun.results = line.strip()
														else:
															try:
																testcase = resultParser.parseResultLine(line)
															except RandomOutput as e:
																#print( "E:" + self.lineNumber + ", " + e.output )
																testRun.output = e.output
																testRun.outputLine = lineNumber - len(e.output)
															if isinstance(testcase, TestCaseData):
																testRun.testcases.append(testcase)
															if line.startswith("==============================================================================="):
																endOfRun = True
															lineNumber += 1
					lines = testRun.generateSortedUnapprovedLines(testRun.outputLine)
		
					rawSortedPath = os.path.join( rootPath, '{0}.sorted.unapproved.txt'.format( baseName ) )
					rawWriteFile = open( rawSortedPath, 'wb' )
					for line in lines:
						#print( "L:" + line )
						rawWriteFile.write(line + "\n")
					rawWriteFile.close()


def approveMsTest( baseName, filter ):
	rawResultsPath = os.path.join( rootPath, '_{0}.tmp'.format( baseName ) )
	if not(os.path.exists( dllPath )):
		raise Exception("Managed DLL does not exist: '" + dllPath + "'")

	args = []
	# Options for VS2010
	args.append("MSTest.exe")
	args.append("/testcontainer:" + dllPath)
	args.append("/category:\"" + filter + "\"")
	
	# Options for VS2012 managed
	#args.append("vstest.console.exe")
	#args.append("/Logger:Trx")
	#args.append(dllPath)
	#args.append("/TestCaseFilter:TestCategory=" + filter)

	# Options for VS2012 native
	#args.append("vstest.console.exe")
	#args.append("/Logger:Trx")
	#args.append(dllPath)
	#args.append("/TestCaseFilter:Owner=" + filter)
	
	#print( args )
	f = open( rawResultsPath, 'w' )
	subprocess.call( args, stdout=f, stderr=f )
	f.close()

	if os.path.exists( rawResultsPath ):
		f = open( rawResultsPath, 'r' )
		for line in f:
			if line.startswith("Results file:") or line.startswith("Results File:"):
				trxFile = line[13:].strip()
				parseTrxFile(baseName, trxFile)
		f.close()
		os.remove( rawResultsPath )

# Standard console reporter
approve( "console.std", ["~_"] )
# console reporter, include passes, warn about No Assertions
approve( "console.sw", ["~_", "-s", "-w", "NoAssertions"] )
# console reporter, include passes, warn about No Assertions, limit failures to first 4
approve( "console.swa4", ["~_", "-s", "-w", "NoAssertions", "-x", "4"] )
# junit reporter, include passes, warn about No Assertions
approveJunit( "junit.sw", ["~_", "-s", "-w", "NoAssertions", "-r", "junit"] )
# xml reporter, include passes, warn about No Assertions
approveXml( "xml.sw", ["~_", "-s", "-w", "NoAssertions", "-r", "xml"] )
# mstest runner, xml output
approveMsTest( "mstest.std", "all")
approveMsTest( "mstest.sw", "allSucceeding")
approveMsTest( "mstest.swa4", "allSucceedingAborting")

if overallResult != 0:
	print( "run approve.py to approve new baselines" )
exit( overallResult)

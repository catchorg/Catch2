VS2012 supports MSTest for Managed tests and vstest.console.exe for Native tests. 

## Managed Tests

To run managed tests from the command line:

```
C:\Projects\VS2012ManagedTestExample>"c:\Program Files (x86)\Microsoft Visual Studio 11.0\Common7\IDE\MSTest.exe" /testcontainer:Debug\DefaultTest.dll
Microsoft (R) Test Execution Command Line Tool Version 11.0.50727.1
Copyright (c) Microsoft Corporation. All rights reserved.

Loading Debug\DefaultTest.dll...
Starting execution...

Results               Top Level Tests
-------               ---------------
Failed                UnitTest1.?A0x066a5656.C_A_T_C_H____T_E_S_T____C_L_A_S_S___10_1.C_A_T_C_H___M_E_T_H_O_D___10_1
Passed                UnitTest1.?A0x066a5656.C_A_T_C_H____T_E_S_T____C_L_A_S_S___6_0.C_A_T_C_H___M_E_T_H_O_D___6_0
1/2 test(s) Passed, 1 Failed

Summary
-------
Test Run Failed.
  Failed  1
  Passed  1
  ---------
  Total   2
Results file:  C:\Projects\VS2012ManagedTestExample\TestResults\user_machine 2013-11-18 21_50_39.trx
Test Settings: Default Test Settings
```
which gives the following output in the .trx file:

```
<?xml version="1.0" encoding="UTF-8"?>
<TestRun id="4cd5aa0a-c9ca-49be-b248-d8bdff17734f" name="Malcolm@MAGNESIUM 2013-11-18 21:50:39" runUser="MAGNESIUM\Malcolm" xmlns="http://microsoft.com/schemas/VisualStudio/TeamTest/2010">
  <TestSettings name="Default Test Settings" id="77f2b0c1-8e12-4f70-8ef1-c7370357d694">
    <Execution>
      <TestTypeSpecific />
      <AgentRule name="Execution Agents">
      </AgentRule>
    </Execution>
    <Deployment runDeploymentRoot="Malcolm_MAGNESIUM 2013-11-18 21_50_39" />
  </TestSettings>
  <Times creation="2013-11-18T21:50:39.6398000+00:00" queuing="2013-11-18T21:50:40.5708000+00:00" start="2013-11-18T21:50:40.7648000+00:00" finish="2013-11-18T21:50:42.1828000+00:00" />
  <ResultSummary outcome="Failed">
    <Counters total="2" executed="2" passed="1" error="0" failed="1" timeout="0" aborted="0" inconclusive="0" passedButRunAborted="0" notRunnable="0" notExecuted="0" disconnected="0" warning="0" completed="0" inProgress="0" pending="0" />
  </ResultSummary>
  <TestDefinitions>
    <UnitTest name="C_A_T_C_H___M_E_T_H_O_D___6_0" storage="debug\defaulttest.dll" id="602e9cef-23af-97c5-09fb-9d08a9431a51">
      <Description>My first catch test in VS2010</Description>
      <Execution id="a5de9acd-c39a-46ab-8779-df5665bc5de1" />
      <Properties>
        <Property>
          <Key>Description</Key>
          <Value>My first catch test in VS2010</Value>
        </Property>
      </Properties>
      <TestMethod codeBase="C:/Projects/VS2012ManagedTestExample/Debug/DefaultTest.DLL" adapterTypeName="Microsoft.VisualStudio.TestTools.TestTypes.Unit.UnitTestAdapter, Microsoft.VisualStudio.QualityTools.Tips.UnitTest.Adapter, Version=11.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a" className="UnitTest1.?A0x066a5656.C_A_T_C_H____T_E_S_T____C_L_A_S_S___6_0, DefaultTest, Version=1.0.5069.28570, Culture=neutral, PublicKeyToken=null" name="C_A_T_C_H___M_E_T_H_O_D___6_0" />
    </UnitTest>
    <UnitTest name="C_A_T_C_H___M_E_T_H_O_D___10_1" storage="debug\defaulttest.dll" id="f193e129-90c0-27cd-511d-972d38fc323a">
      <Description>My first failing catch test in VS2010</Description>
      <Execution id="f2e6a046-5e7b-4606-97a8-7faac3ff61ed" />
      <Properties>
        <Property>
          <Key>Description</Key>
          <Value>My first failing catch test in VS2010</Value>
        </Property>
      </Properties>
      <TestMethod codeBase="C:/Projects/VS2012ManagedTestExample/Debug/DefaultTest.DLL" adapterTypeName="Microsoft.VisualStudio.TestTools.TestTypes.Unit.UnitTestAdapter, Microsoft.VisualStudio.QualityTools.Tips.UnitTest.Adapter, Version=11.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a" className="UnitTest1.?A0x066a5656.C_A_T_C_H____T_E_S_T____C_L_A_S_S___10_1, DefaultTest, Version=1.0.5069.28570, Culture=neutral, PublicKeyToken=null" name="C_A_T_C_H___M_E_T_H_O_D___10_1" />
    </UnitTest>
  </TestDefinitions>
  <TestLists>
    <TestList name="Results Not in a List" id="8c84fa94-04c1-424b-9868-57a2d4851a1d" />
    <TestList name="All Loaded Results" id="19431567-8539-422a-85d7-44ee4e166bda" />
  </TestLists>
  <TestEntries>
    <TestEntry testId="f193e129-90c0-27cd-511d-972d38fc323a" executionId="f2e6a046-5e7b-4606-97a8-7faac3ff61ed" testListId="8c84fa94-04c1-424b-9868-57a2d4851a1d" />
    <TestEntry testId="602e9cef-23af-97c5-09fb-9d08a9431a51" executionId="a5de9acd-c39a-46ab-8779-df5665bc5de1" testListId="8c84fa94-04c1-424b-9868-57a2d4851a1d" />
  </TestEntries>
  <Results>
    <UnitTestResult executionId="f2e6a046-5e7b-4606-97a8-7faac3ff61ed" testId="f193e129-90c0-27cd-511d-972d38fc323a" testName="C_A_T_C_H___M_E_T_H_O_D___10_1" computerName="MAGNESIUM" duration="00:00:00.5287967" startTime="2013-11-18T21:50:40.8238000+00:00" endTime="2013-11-18T21:50:42.0128000+00:00" testType="13cdc9d9-ddb5-4fa4-a97d-d965ccfc6d4b" outcome="Failed" testListId="8c84fa94-04c1-424b-9868-57a2d4851a1d" relativeResultsDirectory="f2e6a046-5e7b-4606-97a8-7faac3ff61ed">
      <Output>
        <StdOut>~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Using Catch v1.0 b13 for a managed MSTest project.
-------------------------------------------------------------------------------
My first failing catch test in VS2010
-------------------------------------------------------------------------------
UnitTest.cpp(10)
...............................................................................

UnitTest.cpp(12): FAILED:
  REQUIRE( false )

===============================================================================
0 test cases - all failed</StdOut>
        <ErrorInfo>
          <Message>Assert.Fail failed. REQUIRE( false ), </Message>
          <StackTrace>   at UnitTest1.?A0x066a5656.C_A_T_C_H____T_E_S_T____10_1() in c:\projects\vs2012managedtestexample\vs2012managedtestexample\unittest.cpp:line 12
   at Catch.FreeFunctionTestCase.invoke(FreeFunctionTestCase* ) in c:\projects\phil\catch\include\internal\catch_vs_test_registry.hpp:line 32
   at Catch.TestCase.invoke(TestCase* ) in c:\projects\phil\catch\include\internal\catch_test_case_info.hpp:line 78
   at Catch.RunContext.runCurrentTest(RunContext* , basic_string&lt;char\,std::char_traits&lt;char&gt;\,std::allocator&lt;char&gt; &gt;* redirectedCout, basic_string&lt;char\,std::char_traits&lt;char&gt;\,std::allocator&lt;char&gt; &gt;* redirectedCerr) in c:\projects\phil\catch\include\internal\catch_runner_impl.hpp:line 282
   at Catch.RunContext.runTest(RunContext* , Totals* , TestCase* testCase) in c:\projects\phil\catch\include\internal\catch_runner_impl.hpp:line 122
   at UnitTest1.?A0x066a5656.C_A_T_C_H____T_E_S_T____C_L_A_S_S___10_1.C_A_T_C_H___M_E_T_H_O_D___10_1() in c:\projects\vs2012managedtestexample\vs2012managedtestexample\unittest.cpp:line 10
</StackTrace>
        </ErrorInfo>
      </Output>
    </UnitTestResult>
    <UnitTestResult executionId="a5de9acd-c39a-46ab-8779-df5665bc5de1" testId="602e9cef-23af-97c5-09fb-9d08a9431a51" testName="C_A_T_C_H___M_E_T_H_O_D___6_0" computerName="MAGNESIUM" duration="00:00:00.0298274" startTime="2013-11-18T21:50:42.0248000+00:00" endTime="2013-11-18T21:50:42.0858000+00:00" testType="13cdc9d9-ddb5-4fa4-a97d-d965ccfc6d4b" outcome="Passed" testListId="8c84fa94-04c1-424b-9868-57a2d4851a1d" relativeResultsDirectory="a5de9acd-c39a-46ab-8779-df5665bc5de1">
    </UnitTestResult>
  </Results>
</TestRun>
```

## Native Tests

Native have to be run using vstest.console.exe:

```
C:\Projects\VS2012NativeTestExample>"C:\Program Files (x86)\Microsoft Visual Studio 11.0\Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe" /Logger:Trx debug\VS2012NativeTestExample.dll
Microsoft (R) Test Execution Command Line Tool Version 11.0.60315.1
Copyright (c) Microsoft Corporation.  All rights reserved.

Starting test execution, please wait...
Information: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Using Catch v1.0 b13 for a native MSTest project.
-------------------------------------------------------------------------------
"My first failing catch test in VS2010"
-------------------------------------------------------------------------------
c:\projects\vs2012nativetestexample\vs2012nativetestexample\unittest1.cpp(10)
...............................................................................

c:\projects\vs2012nativetestexample\vs2012nativetestexample\unittest1.cpp(12): FAILED:
  REQUIRE( false )

===============================================================================
0 test cases - all failed



Passed   C_A_T_C_H___M_E_T_H_O_D___6_0
Failed   C_A_T_C_H___M_E_T_H_O_D___10_1
Error Message:
   Assert failed. false, . c:\projects\vs2012nativetestexample\vs2012nativetestexample\unittest1.cpp, "My first failing catch test in VS2010"   line 12
Stack Trace:
        at UnitTest1::C_A_T_C_H____T_E_S_T____10_1() in c:\projects\vs2012nativetestexample\vs2012nativetestexample\unittest1.cpp:line 12
        at Catch::FreeFunctionTestCase::invoke() in c:\projects\phil\catch\include\internal\catch_vs_test_registry.hpp:line 32
        at Catch::TestCase::invoke() in c:\projects\phil\catch\include\internal\catch_test_case_info.hpp:line 78
        at Catch::RunContext::runCurrentTest() in c:\projects\phil\catch\include\internal\catch_runner_impl.hpp:line 274

        at Catch::RunContext::runTest() in c:\projects\phil\catch\include\internal\catch_runner_impl.hpp:line 124
        at UnitTest1::C_A_T_C_H___N_S_10_1::C_A_T_C_H____T_E_S_T____C_L_A_S_S___10_1::C_A_T_C_H___M_E_T_H_O_D___10_1() in c:\projects\vs2012nativetestexample\vs2012nativetestexample\unittest1.cpp:line 10Standard Output Messages:
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   Using Catch v1.0 b13 for a native MSTest project.
   -------------------------------------------------------------------------------
   "My first failing catch test in VS2010"
   -------------------------------------------------------------------------------
   c:\projects\vs2012nativetestexample\vs2012nativetestexample\unittest1.cpp(10)
   ...............................................................................

   c:\projects\vs2012nativetestexample\vs2012nativetestexample\unittest1.cpp(12): FAILED:
     REQUIRE( false )

   ===============================================================================
   0 test cases - all failed


Results File: C:\Projects\VS2012NativeTestExample\TestResults\user_machine 2013-11-18 21_53_40.trx

Total tests: 2. Passed: 1. Failed: 1. Skipped: 0.
Test Run Failed.
Test execution time: 1.5498 Seconds
```

which gives results like this in the .trx file:

```
<?xml version="1.0" encoding="UTF-8"?>
<TestRun id="350652af-9463-47d9-b232-9c6891ccadb4" name="Malcolm@MAGNESIUM 2013-11-18 21:53:40" runUser="MAGNESIUM\Malcolm" xmlns="http://microsoft.com/schemas/VisualStudio/TeamTest/2010">
  <Times creation="2013-11-18T21:53:40.9958000+00:00" queuing="2013-11-18T21:53:41.0068000+00:00" start="2013-11-18T21:53:41.0228000+00:00" finish="2013-11-18T21:53:41.1668000+00:00" />
  <TestSettings name="default" id="f940e3ba-7a6e-4241-82ba-119a71c1bd7f">
    <Execution>
      <TestTypeSpecific />
    </Execution>
    <Deployment runDeploymentRoot="Malcolm_MAGNESIUM 2013-11-18 21_53_40" />
  </TestSettings>
  <Results>
    <UnitTestResult executionId="a3e41c86-4633-4714-8ccf-e29fabda77b7" testId="b47a1138-752a-f65f-694f-1607b17696e1" testName="C_A_T_C_H___M_E_T_H_O_D___6_0" computerName="MAGNESIUM" duration="00:00:00.0321040" startTime="2013-11-18T21:53:40.0248000+00:00" endTime="2013-11-18T21:53:40.0569040+00:00" testType="13cdc9d9-ddb5-4fa4-a97d-d965ccfc6d4b" outcome="Passed" testListId="8c84fa94-04c1-424b-9868-57a2d4851a1d" relativeResultsDirectory="a3e41c86-4633-4714-8ccf-e29fabda77b7" />
    <UnitTestResult executionId="4525e3fb-6fa4-4e6a-88a3-57f854d57a20" testId="c5c70289-b8cd-b03e-4930-f1394234ca3e" testName="C_A_T_C_H___M_E_T_H_O_D___10_1" computerName="MAGNESIUM" duration="00:00:00.8082727" startTime="2013-11-18T21:53:40.0828000+00:00" endTime="2013-11-18T21:53:40.8910727+00:00" testType="13cdc9d9-ddb5-4fa4-a97d-d965ccfc6d4b" outcome="Failed" testListId="8c84fa94-04c1-424b-9868-57a2d4851a1d" relativeResultsDirectory="4525e3fb-6fa4-4e6a-88a3-57f854d57a20">
      <Output>
        <StdOut>~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Using Catch v1.0 b13 for a native MSTest project.
-------------------------------------------------------------------------------
"My first failing catch test in VS2010"
-------------------------------------------------------------------------------
c:\projects\vs2012nativetestexample\vs2012nativetestexample\unittest1.cpp(10)
...............................................................................

c:\projects\vs2012nativetestexample\vs2012nativetestexample\unittest1.cpp(12): FAILED:
  REQUIRE( false )

===============================================================================
0 test cases - all failed</StdOut>
        <ErrorInfo>
          <Message>Assert failed. false, . c:\projects\vs2012nativetestexample\vs2012nativetestexample\unittest1.cpp, "My first failing catch test in VS2010"	line 12</Message>
          <StackTrace>	at UnitTest1::C_A_T_C_H____T_E_S_T____10_1() in c:\projects\vs2012nativetestexample\vs2012nativetestexample\unittest1.cpp:line 12
	at Catch::FreeFunctionTestCase::invoke() in c:\projects\phil\catch\include\internal\catch_vs_test_registry.hpp:line 32
	at Catch::TestCase::invoke() in c:\projects\phil\catch\include\internal\catch_test_case_info.hpp:line 78
	at Catch::RunContext::runCurrentTest() in c:\projects\phil\catch\include\internal\catch_runner_impl.hpp:line 274
	at Catch::RunContext::runTest() in c:\projects\phil\catch\include\internal\catch_runner_impl.hpp:line 124
	at UnitTest1::C_A_T_C_H___N_S_10_1::C_A_T_C_H____T_E_S_T____C_L_A_S_S___10_1::C_A_T_C_H___M_E_T_H_O_D___10_1() in c:\projects\vs2012nativetestexample\vs2012nativetestexample\unittest1.cpp:line 10</StackTrace>
        </ErrorInfo>
      </Output>
    </UnitTestResult>
  </Results>
  <TestDefinitions>
    <UnitTest name="C_A_T_C_H___M_E_T_H_O_D___6_0" storage="c:\projects\vs2012nativetestexample\debug\vs2012nativetestexample.dll" id="b47a1138-752a-f65f-694f-1607b17696e1">
      <Execution id="a3e41c86-4633-4714-8ccf-e29fabda77b7" />
      <TestMethod codeBase="C:\Projects\VS2012NativeTestExample\debug\VS2012NativeTestExample.dll" adapterTypeName="Microsoft.VisualStudio.TestTools.TestTypes.Unit.UnitTestAdapter" className="DefaultClassName" name="C_A_T_C_H___M_E_T_H_O_D___6_0" />
    </UnitTest>
    <UnitTest name="C_A_T_C_H___M_E_T_H_O_D___10_1" storage="c:\projects\vs2012nativetestexample\debug\vs2012nativetestexample.dll" id="c5c70289-b8cd-b03e-4930-f1394234ca3e">
      <Execution id="4525e3fb-6fa4-4e6a-88a3-57f854d57a20" />
      <TestMethod codeBase="C:\Projects\VS2012NativeTestExample\debug\VS2012NativeTestExample.dll" adapterTypeName="Microsoft.VisualStudio.TestTools.TestTypes.Unit.UnitTestAdapter" className="DefaultClassName" name="C_A_T_C_H___M_E_T_H_O_D___10_1" />
    </UnitTest>
  </TestDefinitions>
  <TestEntries>
    <TestEntry testId="b47a1138-752a-f65f-694f-1607b17696e1" executionId="a3e41c86-4633-4714-8ccf-e29fabda77b7" testListId="8c84fa94-04c1-424b-9868-57a2d4851a1d" />
    <TestEntry testId="c5c70289-b8cd-b03e-4930-f1394234ca3e" executionId="4525e3fb-6fa4-4e6a-88a3-57f854d57a20" testListId="8c84fa94-04c1-424b-9868-57a2d4851a1d" />
  </TestEntries>
  <TestLists>
    <TestList name="Results Not in a List" id="8c84fa94-04c1-424b-9868-57a2d4851a1d" />
    <TestList name="All Loaded Results" id="19431567-8539-422a-85d7-44ee4e166bda" />
  </TestLists>
  <ResultSummary outcome="Failed">
    <Counters total="2" executed="2" passed="1" failed="1" error="0" timeout="0" aborted="0" inconclusive="0" passedButRunAborted="0" notRunnable="0" notExecuted="0" disconnected="0" warning="0" completed="0" inProgress="0" pending="0" />
    <Output>
      <StdOut>~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Using Catch v1.0 b13 for a native MSTest project.
-------------------------------------------------------------------------------
"My first failing catch test in VS2010"
-------------------------------------------------------------------------------
c:\projects\vs2012nativetestexample\vs2012nativetestexample\unittest1.cpp(10)
...............................................................................

c:\projects\vs2012nativetestexample\vs2012nativetestexample\unittest1.cpp(12): FAILED:
  REQUIRE( false )

===============================================================================
0 test cases - all failed

</StdOut>
    </Output>
  </ResultSummary>
</TestRun>
```

---

[Home](../../README.md)
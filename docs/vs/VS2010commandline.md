VS2010 supports MSTest from the command line:

```
C:\Projects\VS2010TestProjectExample>"c:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE\MSTest.exe" /testcontainer:Debug\DefaultTest.dll
Microsoft (R) Test Execution Command Line Tool Version 10.0.30319.1
Copyright (c) Microsoft Corporation. All rights reserved.

Loading Debug\DefaultTest.dll...
Starting execution...

Results               Top Level Tests
-------               ---------------
Failed                UnitTest1.?A0x0bae6f70.C_A_T_C_H____T_E_S_T____C_L_A_S_S___10_1.C_A_T_C_H___M_E_T_H_O_D___10_1
Passed                UnitTest1.?A0x0bae6f70.C_A_T_C_H____T_E_S_T____C_L_A_S_S___6_0.C_A_T_C_H___M_E_T_H_O_D___6_0
1/2 test(s) Passed, 1 Failed

Summary
-------
Test Run Failed.
  Failed  1
  Passed  1
  ---------
  Total   2
Results file:  C:\Projects\VS2010TestProjectExample\TestResults\user_machine 2013-11-18 21_43_26.trx
Test Settings: Default Test Settings
```
which gives the following output in the .trx file:

```
<?xml version="1.0" encoding="UTF-8"?>
<TestRun id="e51ef9c4-994d-430e-ad5f-0d9ae7cfe913" name="Malcolm@MAGNESIUM 2013-11-18 21:43:26" runUser="MAGNESIUM\Malcolm" xmlns="http://microsoft.com/schemas/VisualStudio/TeamTest/2010">
  <TestSettings name="Default Test Settings" id="2a054e79-81e4-409c-98ab-52ce7c45d869">
    <Execution>
      <TestTypeSpecific />
      <AgentRule name="Execution Agents">
      </AgentRule>
    </Execution>
    <Deployment runDeploymentRoot="Malcolm_MAGNESIUM 2013-11-18 21_43_26" />
  </TestSettings>
  <Times creation="2013-11-18T21:43:26.3078000+00:00" queuing="2013-11-18T21:43:27.4898000+00:00" start="2013-11-18T21:43:27.6058000+00:00" finish="2013-11-18T21:43:28.7518000+00:00" />
  <ResultSummary outcome="Failed">
    <Counters total="2" executed="2" passed="1" error="0" failed="1" timeout="0" aborted="0" inconclusive="0" passedButRunAborted="0" notRunnable="0" notExecuted="0" disconnected="0" warning="0" completed="0" inProgress="0" pending="0" />
  </ResultSummary>
  <TestDefinitions>
    <UnitTest name="C_A_T_C_H___M_E_T_H_O_D___10_1" storage="debug\defaulttest.dll" id="af655967-2bb2-3e7d-ea29-dcb935212eee">
      <Description>My first failing catch test in VS2010</Description>
      <Execution id="68d93196-6df0-4039-9cdd-18b119757895" />
      <Properties>
        <Property>
          <Key>Description</Key>
          <Value>My first failing catch test in VS2010</Value>
        </Property>
      </Properties>
      <TestMethod codeBase="C:/Projects/VS2010TestProjectExample/Debug/DefaultTest.DLL" adapterTypeName="Microsoft.VisualStudio.TestTools.TestTypes.Unit.UnitTestAdapter, Microsoft.VisualStudio.QualityTools.Tips.UnitTest.Adapter, Version=10.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a" className="UnitTest1.?A0x0bae6f70.C_A_T_C_H____T_E_S_T____C_L_A_S_S___10_1, DefaultTest, Version=1.0.5069.27763, Culture=neutral, PublicKeyToken=null" name="C_A_T_C_H___M_E_T_H_O_D___10_1" />
    </UnitTest>
    <UnitTest name="C_A_T_C_H___M_E_T_H_O_D___6_0" storage="debug\defaulttest.dll" id="42f20899-dc83-75ef-7fff-7375b45879fe">
      <Description>My first catch test in VS2010</Description>
      <Execution id="5e53497d-efab-4d09-8ae5-5235e945126d" />
      <Properties>
        <Property>
          <Key>Description</Key>
          <Value>My first catch test in VS2010</Value>
        </Property>
      </Properties>
      <TestMethod codeBase="C:/Projects/VS2010TestProjectExample/Debug/DefaultTest.DLL" adapterTypeName="Microsoft.VisualStudio.TestTools.TestTypes.Unit.UnitTestAdapter, Microsoft.VisualStudio.QualityTools.Tips.UnitTest.Adapter, Version=10.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a" className="UnitTest1.?A0x0bae6f70.C_A_T_C_H____T_E_S_T____C_L_A_S_S___6_0, DefaultTest, Version=1.0.5069.27763, Culture=neutral, PublicKeyToken=null" name="C_A_T_C_H___M_E_T_H_O_D___6_0" />
    </UnitTest>
  </TestDefinitions>
  <TestLists>
    <TestList name="Results Not in a List" id="8c84fa94-04c1-424b-9868-57a2d4851a1d" />
    <TestList name="All Loaded Results" id="19431567-8539-422a-85d7-44ee4e166bda" />
  </TestLists>
  <TestEntries>
    <TestEntry testId="af655967-2bb2-3e7d-ea29-dcb935212eee" executionId="68d93196-6df0-4039-9cdd-18b119757895" testListId="8c84fa94-04c1-424b-9868-57a2d4851a1d" />
    <TestEntry testId="42f20899-dc83-75ef-7fff-7375b45879fe" executionId="5e53497d-efab-4d09-8ae5-5235e945126d" testListId="8c84fa94-04c1-424b-9868-57a2d4851a1d" />
  </TestEntries>
  <Results>
    <UnitTestResult executionId="68d93196-6df0-4039-9cdd-18b119757895" testId="af655967-2bb2-3e7d-ea29-dcb935212eee" testName="C_A_T_C_H___M_E_T_H_O_D___10_1" computerName="MAGNESIUM" duration="00:00:00.3607175" startTime="2013-11-18T21:43:27.6318000+00:00" endTime="2013-11-18T21:43:28.6548000+00:00" testType="13cdc9d9-ddb5-4fa4-a97d-d965ccfc6d4b" outcome="Failed" testListId="8c84fa94-04c1-424b-9868-57a2d4851a1d" relativeResultsDirectory="68d93196-6df0-4039-9cdd-18b119757895">
      <Output>
        <StdOut>~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Using Catch v1.0 b13 for a managed MSTest project.
-------------------------------------------------------------------------------
My first failing catch test in VS2010
-------------------------------------------------------------------------------
UnitTest1.cpp(10)
...............................................................................

UnitTest1.cpp(12): FAILED:
  REQUIRE( false )

===============================================================================
0 test cases - all failed</StdOut>
        <ErrorInfo>
          <Message>Assert.Fail failed. REQUIRE( false ), </Message>
          <StackTrace>   at UnitTest1.?A0x0bae6f70.C_A_T_C_H____T_E_S_T____10_1() in c:\projects\vs2010testprojectexample\vs2010testprojectexample\unittest1.cpp:line 12
   at Catch.FreeFunctionTestCase.invoke(FreeFunctionTestCase* ) in c:\projects\phil\catch\include\internal\catch_vs_test_registry.hpp:line 32
   at Catch.TestCase.invoke(TestCase* ) in c:\projects\phil\catch\include\internal\catch_test_case_info.hpp:line 78
   at Catch.RunContext.runCurrentTest(RunContext* , basic_string&lt;char\,std::char_traits&lt;char&gt;\,std::allocator&lt;char&gt; &gt;* redirectedCout, basic_string&lt;char\,std::char_traits&lt;char&gt;\,std::allocator&lt;char&gt; &gt;* redirectedCerr) in c:\projects\phil\catch\include\internal\catch_runner_impl.hpp:line 282
   at Catch.RunContext.runTest(RunContext* , Totals* , TestCase* testCase) in c:\projects\phil\catch\include\internal\catch_runner_impl.hpp:line 122
   at UnitTest1.?A0x0bae6f70.C_A_T_C_H____T_E_S_T____C_L_A_S_S___10_1.C_A_T_C_H___M_E_T_H_O_D___10_1() in c:\projects\vs2010testprojectexample\vs2010testprojectexample\unittest1.cpp:line 10
</StackTrace>
        </ErrorInfo>
      </Output>
    </UnitTestResult>
    <UnitTestResult executionId="5e53497d-efab-4d09-8ae5-5235e945126d" testId="42f20899-dc83-75ef-7fff-7375b45879fe" testName="C_A_T_C_H___M_E_T_H_O_D___6_0" computerName="MAGNESIUM" duration="00:00:00.0318671" startTime="2013-11-18T21:43:28.6618000+00:00" endTime="2013-11-18T21:43:28.6988000+00:00" testType="13cdc9d9-ddb5-4fa4-a97d-d965ccfc6d4b" outcome="Passed" testListId="8c84fa94-04c1-424b-9868-57a2d4851a1d" relativeResultsDirectory="5e53497d-efab-4d09-8ae5-5235e945126d">
    </UnitTestResult>
  </Results>
</TestRun>
```


---

[Home](../../README.md)
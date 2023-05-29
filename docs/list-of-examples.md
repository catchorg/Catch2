<a id="top"></a>
# List of examples

## Already available

### Test Case: Single-file
```cpp
--8<-- "examples/010-TestCase.cpp"
```
### Test Case: Multiple-files
=== "File 1"
    ```cpp
    --8<-- "examples/020-TestCase-1.cpp"
    ```
=== "File 2"
    ```cpp
    --8<-- "examples/020-TestCase-2.cpp"
    ```
### Assertion (REQUIRE, CHECK)
```cpp
--8<-- "examples/030-Asn-Require-Check.cpp"
```
### Fixtures 
#### Sections
```cpp
--8<-- "examples/100-Fix-Section.cpp"
```
#### Class-based fixtures
```cpp
--8<-- "examples/110-Fix-ClassFixture.cpp"
```
### BDD (SCENARIO, GIVEN, WHEN, THEN)
```cpp
--8<-- "examples/120-Bdd-ScenarioGivenWhenThen.cpp"
```
### Listener
```cpp
--8<-- "examples/210-Evt-EventListeners.cpp"
```
### Configuration - *Provide your own output streams*
```cpp
--8<-- "examples/231-Cfg-OutputStreams.cpp"
```
### Generators
#### Create your own generator
```cpp
--8<-- "examples/300-Gen-OwnGenerator.cpp"
```
#### Use map to convert types in GENERATE expression
```cpp
--8<-- "examples/301-Gen-MapTypeConversion.cpp"
```
#### Run test with a table of input values
```cpp
--8<-- "examples/302-Gen-Table.cpp"
```
#### Use variables in generator expressions
```cpp
--8<-- "examples/310-Gen-VariablesInGenerators.cpp"
```
#### Use custom variable capture in generator expressions
```cpp
--8<-- "examples/311-Gen-CustomCapture.cpp"
```

??? note "Planned (TBD)"
     
      - Assertion: [REQUIRE_THAT and Matchers](../examples/040-Asn-RequireThat.cpp)
      - Assertion: [REQUIRE_NO_THROW](../examples/050-Asn-RequireNoThrow.cpp)
      - Assertion: [REQUIRE_THROWS](../examples/050-Asn-RequireThrows.cpp)
      - Assertion: [REQUIRE_THROWS_AS](../examples/070-Asn-RequireThrowsAs.cpp)
      - Assertion: [REQUIRE_THROWS_WITH](../examples/080-Asn-RequireThrowsWith.cpp)
      - Assertion: [REQUIRE_THROWS_MATCHES](../examples/090-Asn-RequireThrowsMatches.cpp)
      - Floating point: [Approx - Comparisons](../examples/130-Fpt-Approx.cpp)
      - Logging: [CAPTURE - Capture expression](../examples/140-Log-Capture.cpp)
      - Logging: [INFO - Provide information with failure](../examples/150-Log-Info.cpp)
      - Logging: [WARN - Issue warning](../examples/160-Log-Warn.cpp)
      - Logging: [FAIL, FAIL_CHECK - Issue message and force failure/continue](../examples/170-Log-Fail.cpp)
      - Logging: [SUCCEED - Issue message and continue](../examples/180-Log-Succeed.cpp)
      - Report: [User-defined type](../examples/190-Rpt-ReportUserDefinedType.cpp)
      - Report: [User-defined reporter](../examples/202-Rpt-UserDefinedReporter.cpp)
      - Report: [Automake reporter](../examples/205-Rpt-AutomakeReporter.cpp)
      - Report: [TAP reporter](../examples/206-Rpt-TapReporter.cpp)
      - Report: [Multiple reporter](../examples/208-Rpt-MultipleReporters.cpp)
      - Configuration: [Provide your own main()](../examples/220-Cfg-OwnMain.cpp)
      - Configuration: [Compile-time configuration](../examples/230-Cfg-CompileTimeConfiguration.cpp)
      - Configuration: [Run-time configuration](../examples/240-Cfg-RunTimeConfiguration.cpp)


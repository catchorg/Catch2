<a id="top"></a>
# Test fixtures

Although Catch allows you to group tests together as sections within a test case, it can still be convenient, sometimes, to group them using a more traditional test fixture. Catch fully supports this too. You define the test fixture as a simple structure:

```c++
class UniqueTestsFixture {
  private:
   static int uniqueID;
  protected:
   DBConnection conn;
  public:
   UniqueTestsFixture() : conn(DBConnection::createConnection("myDB")) {
   }
  protected:
   int getID() {
     return ++uniqueID;
   }
 };

 int UniqueTestsFixture::uniqueID = 0;

 TEST_CASE_METHOD(UniqueTestsFixture, "Create Employee/No Name", "[create]") {
   REQUIRE_THROWS(conn.executeSQL("INSERT INTO employee (id, name) VALUES (?, ?)", getID(), ""));
 }
 TEST_CASE_METHOD(UniqueTestsFixture, "Create Employee/Normal", "[create]") {
   REQUIRE(conn.executeSQL("INSERT INTO employee (id, name) VALUES (?, ?)", getID(), "Joe Bloggs"));
 }
```

The two test cases here will create uniquely-named derived classes of UniqueTestsFixture and thus can access the `getID()` protected method and `conn` member variables. This ensures that both the test cases are able to create a DBConnection using the same method (DRY principle) and that any ID's created are unique such that the order that tests are executed does not matter.


Catch2 also provides `TEMPLATE_TEST_CASE_METHOD` that can be used together
with templated fixtures to perform tests for multiple different types.
However, unlike `TEST_CASE_METHOD`, `TEMPLATE_TEST_CASE_METHOD` requires
the tag specification to be non-empty, as it is followed by further macros
arguments.

Also note that, because of limitations of the C++ preprocessor, if you
want to specify a type with multiple template parameters, you need to
enclose it in parentheses, e.g. `std::map<int, std::string>` needs to be
passed as `(std::map<int, std::string>)`.

Example:
```cpp
template< typename T >
struct Template_Fixture {
    Template_Fixture(): m_a(1) {}

    T m_a;
};

TEMPLATE_TEST_CASE_METHOD(Template_Fixture,"A TEMPLATE_TEST_CASE_METHOD based test run that succeeds", "[class][template]", int, float, double) {
    REQUIRE( Template_Fixture<TestType>::m_a == 1 );
}
```

_While there is an upper limit on the number of types you can specify
in single `TEMPLATE_TEST_CASE`, the limit is very high and should not
be encountered in practice._

---

[Home](Readme.md#top)

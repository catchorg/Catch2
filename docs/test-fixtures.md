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

Catch also provides ```TEMPLATE_TEST_CASE_METHOD``` for creating test for the same templated class against multiple types. Usage of ```TEMPLATE_TEST_CASE_METHOD``` is the same as in case of ```TEST_CASE_METHOD```, except it requires _tags_ to be at least empty string due to following list of typenames up to 8 types. At least one type needs to be specified. Type for which test is run is provided as _TestType_.

---

[Home](Readme.md#top)

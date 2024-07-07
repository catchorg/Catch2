<a id="top"></a>
# Test fixtures

## Non-Templated test fixtures

Although Catch allows you to group tests together as [sections within a test case](test-cases-and-sections.md), it can still be convenient, sometimes, to group them using a more traditional test fixture. Catch fully supports this too with 3 different macros for non-templated test fixtures. They are: 

| Macro    | Description |
|----------|-------------|
|1. `TEST_CASE_METHOD(className, ...)`| Creates a uniquely named class which inherits from the class specified by `className`. The test function will be a member of this derived class. An instance of the derived class will be created for every partial run of the test case. |
|2. `METHOD_AS_TEST_CASE(member-function, ...)`| Uses `member-function` as the test function. An instance of the class will be created for each partial run of the test case. |
|3. `TEST_CASE_FIXTURE(className, ...)`| Creates a uniquely named class which inherits from the class specified by `className`. The test function will be a member of this derived class. An instance of the derived class will be created at the start of the test run. That instance will be destroyed once the entire test case has ended. |

### 1. `TEST_CASE_METHOD`


You define a `TEST_CASE_METHOD` test fixture as a simple structure:

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

### 2. `METHOD_AS_TEST_CASE`

`METHOD_AS_TEST_CASE` lets you register a member function of a class as a Catch2 test case. The class will be separately instantiated for each method registered in this way.

```cpp
class TestClass {
    std::string s;

public:
    TestClass()
        :s( "hello" )
    {}

    void testCase() {
        REQUIRE( s == "hello" );
    }
};


METHOD_AS_TEST_CASE( TestClass::testCase, "Use class's method as a test case", "[class]" )
```

This type of fixture is similar to [TEST_CASE_METHOD](#1-test_case_method) except in this case it will directly use the provided class to create an object rather than a derived class.

### 3. `TEST_CASE_FIXTURE`

`TEST_CASE_FIXTURE` behaves in the same way as [TEST_CASE_METHOD](#1-test_case_method) except that there will only be one instance created throughout the entire run of a test case. To demonstrate this have a look at the following example:

```cpp
struct MyFixture{
    int MyInt = 0;
};

TEST_CASE_FIXTURE(MyFixture, "Tests with MyFixture") { 
    
    const int val = MyInt++;

    SECTION("First partial run") {
        REQUIRE(val == 0);
    }

    SECTION("Second partial run") {
        REQUIRE(val == 1);
    }
}
```
This test case will be executed twice as there are two leaf sections. On the first run `val` will be `0` and on the second run `val` will be `1`. This is useful if you would like to share some expensive setup code with all runs of your test case which can't be done at static initialization time.

## Templated test fixtures

Catch2 also provides `TEMPLATE_TEST_CASE_METHOD` and
`TEMPLATE_PRODUCT_TEST_CASE_METHOD` that can be used together
with templated fixtures and templated template fixtures to perform
tests for multiple different types. Unlike `TEST_CASE_METHOD`,
`TEMPLATE_TEST_CASE_METHOD` and `TEMPLATE_PRODUCT_TEST_CASE_METHOD` do
require the tag specification to be non-empty, as it is followed by
further macro arguments.

Also note that, because of limitations of the C++ preprocessor, if you
want to specify a type with multiple template parameters, you need to
enclose it in parentheses, e.g. `std::map<int, std::string>` needs to be
passed as `(std::map<int, std::string>)`.
In the case of `TEMPLATE_PRODUCT_TEST_CASE_METHOD`, if a member of the
type list should consist of more than single type, it needs to be enclosed
in another pair of parentheses, e.g. `(std::map, std::pair)` and
`((int, float), (char, double))`.

Example:
```cpp
template< typename T >
struct Template_Fixture {
    Template_Fixture(): m_a(1) {}

    T m_a;
};

TEMPLATE_TEST_CASE_METHOD(Template_Fixture,
                          "A TEMPLATE_TEST_CASE_METHOD based test run that succeeds",
                          "[class][template]",
                          int, float, double) {
    REQUIRE( Template_Fixture<TestType>::m_a == 1 );
}

template<typename T>
struct Template_Template_Fixture {
    Template_Template_Fixture() {}

    T m_a;
};

template<typename T>
struct Foo_class {
    size_t size() {
        return 0;
    }
};

TEMPLATE_PRODUCT_TEST_CASE_METHOD(Template_Template_Fixture,
                                  "A TEMPLATE_PRODUCT_TEST_CASE_METHOD based test succeeds",
                                  "[class][template]",
                                  (Foo_class, std::vector),
                                  int) {
    REQUIRE( Template_Template_Fixture<TestType>::m_a.size() == 0 );
}
```

_While there is an upper limit on the number of types you can specify
in single `TEMPLATE_TEST_CASE_METHOD` or `TEMPLATE_PRODUCT_TEST_CASE_METHOD`,
the limit is very high and should not be encountered in practice._

## Signature-based parametrised test fixtures

> [Introduced](https://github.com/catchorg/Catch2/issues/1609) in Catch2 2.8.0.

Catch2 also provides `TEMPLATE_TEST_CASE_METHOD_SIG` and `TEMPLATE_PRODUCT_TEST_CASE_METHOD_SIG` to support
fixtures using non-type template parameters. These test cases work similar to `TEMPLATE_TEST_CASE_METHOD` and `TEMPLATE_PRODUCT_TEST_CASE_METHOD`,
with additional positional argument for [signature](test-cases-and-sections.md#signature-based-parametrised-test-cases).

Example:
```cpp
template <int V>
struct Nttp_Fixture{
    int value = V;
};

TEMPLATE_TEST_CASE_METHOD_SIG(
    Nttp_Fixture,
    "A TEMPLATE_TEST_CASE_METHOD_SIG based test run that succeeds",
    "[class][template][nttp]",
    ((int V), V),
    1, 3, 6) {
    REQUIRE(Nttp_Fixture<V>::value > 0);
}

template<typename T>
struct Template_Fixture_2 {
    Template_Fixture_2() {}

    T m_a;
};

template< typename T, size_t V>
struct Template_Foo_2 {
    size_t size() { return V; }
};

TEMPLATE_PRODUCT_TEST_CASE_METHOD_SIG(
    Template_Fixture_2,
    "A TEMPLATE_PRODUCT_TEST_CASE_METHOD_SIG based test run that succeeds",
    "[class][template][product][nttp]",
    ((typename T, size_t S), T, S),
    (std::array, Template_Foo_2),
    ((int,2), (float,6))) {
    REQUIRE(Template_Fixture_2<TestType>{}.m_a.size() >= 2);
}
```

## Template fixtures with types specified in template type lists

Catch2 also provides `TEMPLATE_LIST_TEST_CASE_METHOD` to support template fixtures with types specified in
template type lists like `std::tuple`, `boost::mpl::list` or `boost::mp11::mp_list`. This test case works the same as `TEMPLATE_TEST_CASE_METHOD`,
only difference is the source of types. This allows you to reuse the template type list in multiple test cases.

Example:
```cpp
using MyTypes = std::tuple<int, char, double>;
TEMPLATE_LIST_TEST_CASE_METHOD(Template_Fixture,
                               "Template test case method with test types specified inside std::tuple",
                               "[class][template][list]",
                               MyTypes) {
    REQUIRE( Template_Fixture<TestType>::m_a == 1 );
}
```

---

[Home](Readme.md#top)

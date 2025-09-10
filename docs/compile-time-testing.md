<a id="top"></a>

# Compile time testing

Catch2 provides a way to test code during compilation. This is
useful for ensuring the code does not lead to undefined behavior.

> [!IMPORTANT]
> This feature is only available with **C++17**

The usage is pretty similar to testing at runtime:

| Runtime macro   | Compile time macro        |
|-----------------|---------------------------|
| `SECTION`       | `CONSTEXPR_SECTION`       |
| `REQUIRE`       | `CONSTEXPR_REQUIRE`       |
| `REQUIRE_FALSE` | `CONSTEXPR_REQUIRE_FALSE` |

```c++
TEST_CASE("My compile time test")
{
    CONSTEXPR_SECTION("Use it the same way as SECTION !")
    {
        std::array v = {1, 2, 3, 4, 5};
        CONSTEXPR_REQUIRE( v.size() == 5 );
        CONSTEXPR_REQUIRE( v[2] == 3 );
    
    }; // <-- Don't forget the semicolon here!
}
```

All code inside the `CONSTEXPR_SECTION` will be evaluated during compilation.
If any of the `CONSTEXPR_REQUIRE` or `CONSTEXPR_REQUIRE_FALSE` fails,
it will cause a compilation error.

> [!NOTE]
> The code inside the `CONSTEXPR_SECTION` is never evaluated at runtime.

## Debuging compile time tests

### Investigate

You can check the output of your compiler to find the failing assertion.
The failing line should be highlighted somewhere.

### Investigate at runtime

If you want to debug the code in a `CONSTEXPR_SECTION`, you can simply replace
the `CONSTEXPR_SECTION` with a `SECTION` and the code will be evaluated at runtime
instead.

## Compile time test report

At runtime, a `CONSTEXPR_SECTION` will add a section in the test report. These sections run nothing, but they
provide a way to see what was tested during compilation in the test report.

For instance, given the following test case:

```C++
TEST_CASE("My other compile time test")
{
    SECTION("Any runtime section")
    {
        SECTION("Nested section")
        {
            CONSTEXPR_SECTION("First constexpr section")
            {
                // ...
            };
        };
        
        CONSTEXPR_SECTION("Second constexpr section")
        {
            // ...
        };
        CONSTEXPR_SECTION("Third constexpr section")
        {
            // ...
        }; 
        CONSTEXPR_SECTION("Fourth section")
        {
            // ...
        };           
    }
    
    SECTION("Another runtime section")
    {
        SECTION("Nested runtime section")
        {
            // ...
        }
    }    
}
```

The report would look like this:

- ✅ My other compile time test
    - ✅ Any runtime section
        - ✅ Nested section
            - ✅ [Passed during compilation] First constexpr section
        - ✅ [Passed during compilation] Second constexpr section
        - ✅ [Passed during compilation] Third constexpr section
        - ✅ [Passed during compilation] Fourth constexpr section
    - ✅ Another runtime section
        - ✅ Nested runtime section

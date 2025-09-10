<a id="top"></a>

# Compile-time testing

Catch2 provides a way to test code during compilation. This is
useful for ensuring the code does not lead to undefined behavior.

> [!IMPORTANT]
> This feature is only available with **C++17**

## Why should I test code at compile-time ?

Testing code during compilation is the only way for ensuring the code does
not lead to [undefined behavior](https://en.cppreference.com/w/cpp/language/ub.html).
This is critical to make your program safe, prevent unexpected crashes and worse
consequences of undefined behavior.

## How ?

Catch2 made it easy to test code at compile-time.

The usage is pretty similar to testing at runtime:

| Runtime macro   | Compile time macro        |
|-----------------|---------------------------|
| `SECTION`       | `CONSTEXPR_SECTION`       |
| `REQUIRE`       | `CONSTEXPR_REQUIRE`       |
| `REQUIRE_FALSE` | `CONSTEXPR_REQUIRE_FALSE` |
| `CHECK`         | -                         |
| `CHECK_FALSE`   | -                         |

```c++
TEST_CASE("My compile time test")
{
    CONSTEXPR_SECTION("std::array subscript and size")
    {
        std::array v = {1, 2, 3, 4, 5};
        CONSTEXPR_REQUIRE( v.size() == 5 );
        CONSTEXPR_REQUIRE( v[2] == 3 );
    
    }; // <-- Don't forget the semicolon here!
}
```

All code inside the `CONSTEXPR_SECTION` will be evaluated during compilation.
If any of the `CONSTEXPR_REQUIRE` or `CONSTEXPR_REQUIRE_FALSE` fails,
it will cause a **compilation error**.

> [!WARNING]
> You cannot use `REQUIRE` or `CHECK` inside a `CONSTEXPR_SECTION`.
> Be careful not to mistake `CONSTEXPR_REQUIRE` with `STATIC_REQUIRE`.
> They both concern compilation-time testing, but they do not have the same
> purpose.

> [!WARNING]
> You cannot nest `CONSTEXPR_SECTION`s or put a `SECTION` inside a
> `CONSTEXPR_SECTION`

> [!NOTE]
> The code inside the `CONSTEXPR_SECTION` is also evaluated at
> runtime. This way, it remains debuggable, and it contributes to the code
> coverage analysis.

## What can I test ?

You can test anything that can be evaluated in a `constexpr` function.
This will depend on your compiler and the C++ standard you are using.
C++20, C++23 and C++26 improved a lot the support for `constexpr`.

Take a look at the C++ compiler support on [cppreference](https://en.cppreference.com/w/cpp/compiler_support.html)
to see what you can use.

## Debug failing compile-time tests

### Investigate from the compiler output

You can check the output of your compiler to find the failing assertion.
The failing line should be highlighted somewhere. If you cannot see any
line:

- Ensure the `CONSTEXPR_SECTION` runs only code that can be evaluated
at compile-time. This will depend on your compiler and the C++ standard you
are using.
- Ensure you did not use unsupported Catch2 macros inside the `CONSTEXPR_SECTION`.
The only supported macros are `CONSTEXPR_REQUIRE` and `CONSTEXPR_REQUIRE_FALSE`.
- Ensure the code you wrote doesn't produce undefined behavior (UB cannot 
compile). Reading a range outside its bounds, dereferencing an invalid pointer, 
reading a variable after it has been destroyed, are widespread undefined
behaviors. They will be all caught by these tests.

### Investigate at runtime

> [!TIP]
> If you want to debug the code in a `CONSTEXPR_SECTION`, you can simply replace 
> the `CONSTEXPR_SECTION` with a `SECTION` and the code will be evaluated at runtime 
> only instead. Remember to come back to the `CONSTEXPR_SECTION` once you are 
> done !

## Compile time test report

At runtime, a `CONSTEXPR_SECTION` will add a section in the test report. These sections
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

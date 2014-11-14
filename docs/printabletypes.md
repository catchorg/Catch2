# Making Types Printable

Catch is capable of using user defined print functions and overloads.

## Using ostreams

By default, catch will attempt to display types using an ostream overload is available.

```c++
std::ostream& operator<<(std::ostream&, MySpecialType const&);
```

## using toString
If you don't want your type to work with ostreams, or Catch is unable to find the 
overload you provide, you may alternatively overload `Catch::toString`.  The overload 
must appear **above** the include of `catch.hpp`.  For example, if I want to provide
a print functionality for `std::pair<int, char>` in my test, the top of my test file
would look like this:

```c++
#include <string>
#include <sstream>
#include <utility>

namespace Catch {
std::string toString(std::pair<int, char> const p) {
    std::ostringstream oss;
    oss << '{' << p.first << ", " << p.second << '}';
    return oss.str();
}
}

#include "catch.hpp"
```
Note that the former approach is preferred, and the latter is more error-prone
due to possible typos when typing `Catch` or `toString`

## Default output

If neither of the two conditions has been met, catch will output `{?}` by default.

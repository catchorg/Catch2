# String conversions

Catch needs to be able to convert types you use in assertions and logging expressions into strings (for logging and reporting purposes).
Most built-in or std types are supported out of the box but there are three ways that you can tell Catch how to convert your own types (or other, third-party types) into strings.

## operator << overload for std::ostream

This is the standard way of providing string conversions in C++ - and the chances are you may already provide this for your own purposes. If you're not familiar with this idiom it involves writing a free function of the form:

```
std::ostream& operator << ( std::ostream& os, T const& value ) {
	os << convertMyTypeToString( value );
	return os;
}
```

(where ```T``` is your type and ```convertMyTypeToString``` is where you'll write whatever code is necessary to make your type printable - it doesn't have to be in another function).

You should put this function in the same namespace as your type and it has to be declared before including Catch's header.

## Catch::toString overload

If you don't want to provide an ```operator <<``` overload, or you want to convert your type differently for testing purposes, you can provide an overload for ```Catch::toString()``` for your type.

```
namespace Catch {
	std::string toString( T const& value ) {
		return convertMyTypeToString( value );
	}
}
```

Again ```T``` is your type and ```convertMyTypeToString``` is where you'll write whatever code is necessary to make your type printable. Note that the function must be in the Catch namespace, which itself must be in the global namespace and must be declared _before_ Catch's header is included.

**Please note that overloading `Catch::toString` is currently considered legacy and will not be supported in the next major version of Catch.**

## Catch::StringMaker<T> specialisation
Another way of telling Catch how to convert a type to string is specialising `Catch::StringMaker` template. This allows you to have separate way of stringifying types for Catch, than you have for writing it to a stream and also doesn't require you to declare it before including Catch's header.

```
namespace Catch {
	template<> struct StringMaker<T> {
    	static std::string convert( T const& value ) {
        	return convertMyTypeToString( value ); 
        } 
    }; 
}
```

## Exceptions

By default all exceptions deriving from `std::exception` will be translated to strings by calling the `what()` method. For exception types that do not derive from `std::exception` - or if `what()` does not return a suitable string - use `CATCH_TRANSLATE_EXCEPTION`. This defines a function that takes your exception type, by reference, and returns a string. It can appear anywhere in the code - it doesn't have to be in the same translation unit. For example:

```
CATCH_TRANSLATE_EXCEPTION( MyType& ex ) {
	return ex.message();
}
```

---

[Home](Readme.md)

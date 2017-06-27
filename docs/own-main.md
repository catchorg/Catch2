# Supplying main() yourself

The easiest way to use Catch is to let it supply ```main()``` for you and handle configuring itself from the command line.

This is achieved by writing ```#define CATCH_CONFIG_MAIN``` before the ```#include "catch.hpp"``` in *exactly one* source file.

Sometimes, though, you need to write your own version of main(). You can do this by writing ```#define CATCH_CONFIG_RUNNER``` instead. Now you are free to write ```main()``` as normal and call into Catch yourself manually.

You now have a lot of flexibility - but here are three recipes to get your started:

## Let Catch take full control of args and config

If you just need to have code that executes before and/ or after Catch this is the simplest option.

```c++
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main( int argc, char* argv[] ) {
  // global setup...

  int result = Catch::Session().run( argc, argv );

  // global clean-up...

  return result;
}
```

## Amending the config

If you still want Catch to process the command line, but you want to programatically tweak the config, you can do so in one of two ways:

```c++
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main( int argc, char* argv[] )
{
  Catch::Session session; // There must be exactly one instance

  // writing to session.configData() here sets defaults
  // this is the preferred way to set them
  
  // Verify that all tests, aliases, etc registered properly
  const auto& exceptions = getRegistryHub().getStartupExceptionRegistry().getExceptions();
  if ( !exceptions.empty() ) {
    // iterate over all exceptions and notify user
    for ( const auto& ex_ptr : exceptions ) {
        try {
            std::rethrow_exception(ex_ptr);
        } catch (std::exception const& ex) {
            Catch::cerr() << ex.what();
        }
    }
    // Indicate that an error occured before main
    return 1;
  }
  
  int returnCode = session.applyCommandLine( argc, argv );
  if( returnCode != 0 ) // Indicates a command line error
  	return returnCode;

  // writing to session.configData() or session.Config() here 
  // overrides command line args
  // only do this if you know you need to

  int numFailed = session.run();
  // numFailed is clamped to 255 as some unices only use the lower 8 bits.
  // This clamping has already been applied, so just return it here
  // You can also do any post run clean-up here
  return numFailed;
}
```

Take a look at the definitions of Config and ConfigData to see what you can do with them.

To take full control of the config simply omit the call to ```applyCommandLine()```.

## Adding your own command line options

Catch embeds a powerful command line parser which you can also use to parse your own options out. This capability is still in active development but will be documented here when it is ready.

---

[Home](Readme.md)

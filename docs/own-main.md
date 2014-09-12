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

int main( int argc, char* const argv[] )
{
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

int main( int argc, char* const argv[] )
{
  Catch::Session session; // There must be exactly once instance

  // writing to session.configData() here sets defaults
  // this is the preferred way to set them

  int returnCode = session.applyCommandLine( argc, argv );
  if( returnCode != 0 ) // Indicates a command line error
  	return returnCode;

  // Writing to session.configData() or session.Config() here 
  // overrides command line args.
  // Only do this if you know you need to.

  return session.run();
}
```

Take a look at the definitions of Config and ConfigData to see what you can do with them.

To take full control of the config simply omit the call to ```applyCommandLine()```.

## Adding your own command line options

Catch embeds a powerful command line parser which you can also use to parse your own options out. This capability is still in active development but will be documented here when it is ready.

---

[Home](../README.md)

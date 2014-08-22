# Why do my tests take so long to compile?

Several people have reported that test code written with Catch takes much longer to compile than they would expect. Why is that?

Catch is implemented entirely in headers. There is a little overhead due to this - but not as much as you might think - and you can minimise it simply by organising your test code as follows:

## Short answer
Exactly one source file must ```#define``` either ```CATCH_CONFIG_MAIN``` or ```CATCH_CONFIG_RUNNER``` before ```#include```-ing Catch. In this file *do not write any test cases*! In most cases that means this file will just contain two lines (the ```#define``` and the ```#include```).

## Long answer

Usually C++ code is split between a header file, containing declarations and prototypes, and an implementation file (.cpp) containing the definition, or implementation, code. Each implementation file, along with all the headers that it includes (and which those headers include, etc), is expanded into a single entity called a translation unit - which is then passed to the compiler and compiled down to an object file.

But functions and methods can also be written inline in header files. The downside to this is that these definitions will then be compiled in *every* translation unit that includes the header.

Because Catch is implemented *entirely* in headers you might think that the whole of Catch must be compiled into every translation unit that uses it! Actually it's not quite as bad as that. Catch mitigates this situation by effectively maintaining the traditional separation between the implementation code and declarations. Internally the implementation code is protected by ```#ifdef```s and is conditionally compiled into only one translation unit. This translation unit is that one that ```#define```s ```CATCH_CONFIG_MAIN``` or ```CATCH_CONFIG_RUNNER```. Let's call this the main source file.

As a result the main source file *does* compile the whole of Catch every time! So it makes sense to dedicate this file to *only* ```#define```-ing the identifier and ```#include```-ing Catch (and implementing the runner code, if you're doing that). Keep all your test cases in other files. This way you won't pay the recompilation cost for the whole of Catch 

---

[Home](../README.md)

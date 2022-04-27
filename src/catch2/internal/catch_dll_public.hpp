#ifndef CATCH_DLL_PUBLIC_HPP_INCLUDED
#define CATCH_DLL_PUBLIC_HPP_INCLUDED

#if defined _WIN32 || defined __CYGWIN__
#    ifdef BUILDING_DLL
#        ifdef __GNUC__
#            define CATCH_DLL_PUBLIC __attribute__( ( dllexport ) )
#        else
#            define CATCH_DLL_PUBLIC \
                __declspec( dllexport ) // Note: actually gcc seems to also
                                        // supports this syntax.
#        endif
#    else
#        ifdef __GNUC__
#            define CATCH_DLL_PUBLIC __attribute__( ( dllimport ) )
#        else
#            define CATCH_DLL_PUBLIC \
                __declspec( dllimport ) // Note: actually gcc seems to also
                                        // supports this syntax.
#        endif
#    endif
#    define DLL_LOCAL
#else
#    if __GNUC__ >= 4
#        define CATCH_DLL_PUBLIC __attribute__( ( visibility( "default" ) ) )
#        define DLL_LOCAL __attribute__( ( visibility( "hidden" ) ) )
#    else
#        define CATCH_DLL_PUBLIC
#        define DLL_LOCAL
#    endif
#endif

#endif // CATCH_DLL_PUBLIC_INCLUDED

#!/usr/bin/env python

import json
import os
import urllib2

from scriptCommon import catchPath

def upload(options):
    request = urllib2.Request('http://melpon.org/wandbox/api/compile.json')
    request.add_header('Content-Type', 'application/json')
    response = urllib2.urlopen(request, json.dumps(options))
    return json.loads(response.read())

main_file = '''
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

unsigned int Factorial( unsigned int number ) {
    return number <= 1 ? number : Factorial(number-1)*number;
}

TEST_CASE( "Factorials are computed", "[factorial]" ) {
    REQUIRE( Factorial(1) == 1 );
    REQUIRE( Factorial(2) == 2 );
    REQUIRE( Factorial(3) == 6 );
    REQUIRE( Factorial(10) == 3628800 );
}
'''

def uploadFiles():
    response = upload({
        'compiler': 'gcc-head',
        'code': main_file,
        'codes': [{
            'file': 'catch.hpp',
            'code': open(os.path.join(catchPath, 'single_include', 'catch.hpp')).read()
        }],
        'options': 'c++11,cpp-no-pedantic,boost-nothing',
        'compiler-option-raw': '-DCATCH_CONFIG_FAST_COMPILE',
        'save': True
    })

    if 'status' in response and not 'compiler_error' in response:
        return True, response['url']
    else:
        return False, response

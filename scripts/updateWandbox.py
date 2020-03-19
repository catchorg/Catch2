#!/usr/bin/env python3

import json
import os
import urllib.request
import urllib.parse


from scriptCommon import catchPath

def upload(options):
#    request_blah = urllib.request.Request('https://

    request = urllib.request.Request('https://melpon.org/wandbox/api/compile.json', method='POST')
    json_bytes = json.dumps(options).encode('utf-8')
    request.add_header('Content-Type', 'application/json; charset=utf-8')
    request.add_header('Content-Length', len(json_bytes))
    response = urllib.request.urlopen(request, json_bytes)
    return json.loads(response.read().decode('utf-8'))

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
            'code': open(os.path.join(catchPath, 'single_include', 'catch2', 'catch.hpp')).read()
        }],
        'options': 'c++11,cpp-no-pedantic,boost-nothing',
        'compiler-option-raw': '-DCATCH_CONFIG_FAST_COMPILE',
        'save': True
    })

    if 'url' in response and 'compiler_error' not in response:
        return True, response['url']
    else:
        return False, response

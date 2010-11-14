#import <Foundation/Foundation.h>
#import "../../../catch_runner.hpp"

#import "CatchOCTestCase.h"

int main (int argc, const char * argv[]) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

    // insert code here...
    int result = Catch::Main( argc, (char* const*)argv );
    
    [pool drain];
    return result;
}

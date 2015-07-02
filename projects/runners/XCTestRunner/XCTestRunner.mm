
/*
 * Copyright (c) 2014 - 2015 Landon Fuller <landon@landonf.org>
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include <XCTest/XCTest.h>
#include <inttypes.h>
#include <objc/message.h>

namespace Catch {

static void *TestInfoKey = NULL;
static void *TestSelectorKey = NULL;
static void *TestSessionKey = NULL;

class XCTestRegistryHub : public RegistryHub {
public:
    XCTestRegistryHub (Session &session) : _session(session) {}
    
    /* Generates an XCTest instance that allows Xcode to find and invoke all test cases. */
    virtual void registerTest( TestCase const& testInfo ) {
        /* Create a standard registration via our superclass */
        RegistryHub::registerTest(testInfo);
        
        /* Generate a valid class name from the description */
        const char *description = testInfo.name.c_str();
        NSString *clsName = normalize_identifier([NSString stringWithUTF8String: description]);
        
        /* If the class name is already in use, loop until we've got a unique name */
        if (NSClassFromString(clsName) != nil) {
            /* First, try appending 'Tests'; this handles the case where the tests have the same name as the class being tested. */
            if (![clsName hasSuffix: @"Test"] && ![clsName hasSuffix: @"Tests"]) {
                NSString *testClassName = [clsName stringByAppendingString: @"Tests"];
                
                if (NSClassFromString(testClassName) == nil) {
                    clsName = testClassName;
                }
            }
            
            /* Otherwise, try appending a unique number to the name */
            for (NSUInteger i = 0; i < NSUIntegerMax && NSClassFromString(clsName) != nil; i++) {
                if (i == NSUIntegerMax) {
                    [NSException raise: NSInternalInconsistencyException format: @"Couldn't find a unique test name for %@. You must have an impressive number of tests.", clsName];
                    __builtin_trap();
                }
                
                NSMutableString *proposedName = [NSMutableString stringWithFormat:@"%@%" PRIu64, clsName, (uint64_t) i];
                if (NSClassFromString(proposedName) == nil) {
                    clsName = proposedName;
                    break;
                }
            }
        }
        
        /* Allocate the new class */
        Class cls = objc_allocateClassPair([XCTestCase class], [clsName UTF8String], 0);
        if (cls == nil) {
            [NSException raise: NSInternalInconsistencyException format: @"Could not allocate test class: %@", clsName];
            __builtin_trap();
        }
        
        /* Add XCTestCase methods */
        { // +defaultTestSuite
            Method m = class_getInstanceMethod([[XCTestCase class] class], @selector(defaultTestSuite));
            class_addMethod(object_getClass(cls), @selector(defaultTestSuite), (IMP) _methodImpl_defaultTestSuite, method_getTypeEncoding(m));
        }
        
        /* Add XCTest methods */
        { // -name
            Method m = class_getInstanceMethod([XCTestCase class], @selector(name));
            class_addMethod(cls, @selector(name), (IMP) _methodImpl_name, method_getTypeEncoding(m));
        }
        
        /* Add a test invocation method */
        SEL testSel = register_test_selector(cls, description, _methodImpl_runTest);
        objc_setAssociatedObject(cls, &TestSelectorKey, (__bridge id) (void *) testSel, OBJC_ASSOCIATION_ASSIGN);

        /* Register the new class */
        objc_registerClassPair(cls);

        /* Attach the test info to the class */
        TestCase *tc = new TestCase(testInfo); // Leaks if the class is deregistered (which is unlikely)
        objc_setAssociatedObject(cls, &TestInfoKey, (__bridge id) tc, OBJC_ASSOCIATION_ASSIGN);
        
        objc_setAssociatedObject(cls, &TestSessionKey, (__bridge id) &_session, OBJC_ASSOCIATION_ASSIGN);
    }
    
private:
    /** Our singleton test session */
    Session &_session;
    
    static XCTestSuite *_methodImpl_defaultTestSuite (Class self, SEL _cmd) {
        TestCase const& testInfo = *(__bridge TestCase const *) objc_getAssociatedObject(self, &TestInfoKey);
        SEL testSel = (SEL) (__bridge void *) objc_getAssociatedObject(self, &TestSelectorKey);
        
        XCTestCase *tc = [(XCTestCase *)[self alloc] initWithSelector: testSel];
#if !__has_feature(objc_arc)
        [tc autorelease];
#endif

        XCTestSuite *suite = [XCTestSuite testSuiteWithName: tc.name];
        [suite addTest: tc];
        return suite;
    }
    
    static NSString *_methodImpl_name (Class self, SEL _cmd) {
        TestCase const& testInfo = *(__bridge TestCase const *) objc_getAssociatedObject([self class], &TestInfoKey);
        return [NSString stringWithUTF8String: testInfo.name.c_str()];
    }
    
    static void _methodImpl_runTest (XCTestCase *self, SEL _cmd) {
        TestCase const& testInfo = *(__bridge TestCase const *) objc_getAssociatedObject([self class], &TestInfoKey);
        Session *session = (__bridge Session *) objc_getAssociatedObject([self class], &TestSessionKey);
        
        /* Reset the session's configuration */
        session->useConfigData(ConfigData());
        
        /* Pass our testcase spec to the session */
        auto testSpec = testInfo.name;
        const char *argv0 = [[[[NSProcessInfo processInfo] arguments] objectAtIndex: 0] UTF8String];
        const char * const args[] = { argv0, "--break" /* break on errors */, testSpec.c_str() };
        session->applyCommandLine(sizeof(args) / sizeof(args[0]), (char * const *) args);
        
        /* Run */
        if (session->run() != 0) {
            /* TODO: Do we need our own reporter to provide more useful errors? */
            XCTFail(@"%@ failed", self.name);
        }
    }
    
    
    /* Given a test case description and a target class, derive a unique selector selector from the camel cased description, and register it with the target class and imp. */
    SEL register_test_selector (Class cls, const char *description, void (*imp)(XCTestCase *self, SEL _cmd)) {
        /* Generate a valid selector for the description */
        NSString *selectorName = normalize_identifier([NSString stringWithUTF8String: description]);
        
        /* If the selector is already in use, loop until we have a unique name */
        while (class_getInstanceMethod(cls, NSSelectorFromString(selectorName)) != NULL) {
            for (NSUInteger i = 0; i < NSUIntegerMax; i++) {
                if (i == NSUIntegerMax) {
                    [NSException raise: NSInternalInconsistencyException format: @"Couldn't find a unique selector name for %s. You must have an impressive number of tests.", description];
                    __builtin_trap();
                }
                
                selectorName = [NSString stringWithFormat:@"%@%" PRIu64, selectorName, (uint64_t) i];
                if (class_getInstanceMethod(cls, NSSelectorFromString(selectorName)) == NULL)
                break;
            }
        }
        
        /* Register and return the SEL */
        SEL newSel = NSSelectorFromString(selectorName);
        
        { // -xsmTestSection
            NSString *typeEnc = [NSString stringWithFormat: @"%s%s%s", @encode(void), @encode(id), @encode(SEL)];
            class_addMethod(cls, newSel, (IMP) imp, [typeEnc UTF8String]);
        }
        
        return newSel;
    }

    /* Given a test case description, generate a valid Objective-C identifier. */
    NSString *normalize_identifier (NSString *description) {
        /* Split the description into individual components */
        NSArray *components = [description componentsSeparatedByCharactersInSet: [NSCharacterSet whitespaceAndNewlineCharacterSet]];
        
        /* Camel case any elements that contain only lowercase letters (otherwise, we assume the case is already correct), and remove
         * any invalid characters that may not occur in an Objective-C identifier. */
        NSMutableArray *cleanedComponents = [NSMutableArray arrayWithCapacity: [components count]];
        NSCharacterSet *allowedChars = [NSCharacterSet characterSetWithCharactersInString: @"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"];
        for (NSUInteger i = 0; i < [components count]; i++) {
            NSString *component = components[i];
            NSString *newComponent = [[component componentsSeparatedByCharactersInSet: [allowedChars invertedSet]] componentsJoinedByString: @""];
            
            /* If the string is entirely lowercase, apply camel casing */
            if ([[newComponent lowercaseString] isEqualToString: newComponent]) {
                newComponent = [newComponent capitalizedString];
            }
            
            /* Add to the result array. */
            [cleanedComponents addObject: newComponent];
        }
        
        /* Generate the identifier, stripping any leading numbers */
        NSString *identifier = [cleanedComponents componentsJoinedByString: @""];
        {
            NSRange range = [identifier rangeOfCharacterFromSet: [NSCharacterSet decimalDigitCharacterSet]];
            if (range.location == 0)
            identifier = [identifier substringFromIndex: range.length];
        }
        
        return identifier;
    }
};
    
}

@interface XCTestCaseCatchRegistry : NSObject @end
@implementation XCTestCaseCatchRegistry

/* Runs before all C++ initializers; we can insert our custom registry hub here. */
+ (void) load {
    using namespace Catch;
    static Session session;
    
    RegistryHub **hub = &getTheRegistryHub();
    RegistryHub *previous = *hub;
    *hub = new XCTestRegistryHub(session);
    delete previous;
}

@end

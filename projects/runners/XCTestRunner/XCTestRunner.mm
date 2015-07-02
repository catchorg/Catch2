
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
    
class XCTestReporter : public StreamingReporterBase {
public:
    XCTestReporter (ReporterConfig const &config) : StreamingReporterBase(config) {}
    virtual ~XCTestReporter () {}
    
    const std::vector<AssertionStats> &getCollectedAssertions () { return _collectedAssertions; }
    
private:
    static std::string getDescription() {
        return "Reports test results via Xcode's XCTest interface";
    }

    virtual void assertionStarting (AssertionInfo const &) {}
    
    virtual bool assertionEnded (AssertionStats const &assertionStats) {
        _collectedAssertions.push_back(assertionStats);
        return true;
    }
    
    virtual ReporterPreferences getPreferences() const {
        ReporterPreferences prefs;
        prefs.shouldRedirectStdOut = false;
        return prefs;
    }
    
private:
    std::vector<AssertionStats> _collectedAssertions;
};

class XCTestRegistryHub : public RegistryHub {
private:
    /* Keys used to manage our generated XCTestCase subclass' configuration via
     * objc_setAssociatedObject/objc_getAssociatedObject */
    static const void *TestInfoKey;
    static const void *TestSelectorKey;
    
public:
    XCTestRegistryHub () {}
    
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
    }
    
private:
    /* +[XCTestSuite defaultTestSuite] */
    static XCTestSuite *_methodImpl_defaultTestSuite (Class self, SEL _cmd) {
        TestCase const& testInfo = *(__bridge TestCase const *) objc_getAssociatedObject(self, &TestInfoKey);
        SEL testSel = (SEL) (__bridge void *) objc_getAssociatedObject(self, &TestSelectorKey);
        
        XCTestCase *tc = [(XCTestCase *)[self alloc] initWithSelector: testSel];
#if !CATCH_ARC_ENABLED
        [tc autorelease];
#endif

        XCTestSuite *suite = [XCTestSuite testSuiteWithName: tc.name];
        [suite addTest: tc];
        return suite;
    }
    
    /* -[XCTestSuite name] */
    static NSString *_methodImpl_name (Class self, SEL _cmd) {
        TestCase const& testInfo = *(__bridge TestCase const *) objc_getAssociatedObject([self class], &TestInfoKey);
        return [NSString stringWithUTF8String: testInfo.name.c_str()];
    }
    
    /* -[XCTestSuite <per-testcase-generated-selector>] */
    static void _methodImpl_runTest (XCTestCase *self, SEL _cmd) {
        TestCase const& testInfo = *(__bridge TestCase const *) objc_getAssociatedObject([self class], &TestInfoKey);
        
        /* Target only our configured test case */
        ConfigData data;
        data.testsOrTags.push_back(testInfo.name);
        Ptr<IConfig> config = new Config(data);
        
        /* Set up our run context */
        XCTestReporter* reporter = new XCTestReporter(ReporterConfig(config));
        RunContext runner(config.get(), reporter);
        
        /* Run the test */
        Totals totals;
        runner.testGroupStarting(testInfo.name, 1, 1);
        totals += runner.runTest(testInfo);
        runner.testGroupEnded(testInfo.name, totals, 1, 1);
        
        /* Report failures to XCTest */
        for (auto &&as : reporter->getCollectedAssertions()) {
            auto result = as.assertionResult;
            bool expected = true;
            std::string description = "";
            std::string cause = "";
            
            /* Skip successful tests; we don't need to report anything */
            if (result.getResultType() == ResultWas::Ok)
                continue;
            
            /* Skip expected failures; those are a pass */
            if (result.getResultType() == ResultWas::ExpressionFailed && result.isOk())
                continue;
            
            switch (result.getResultType()) {
                case ResultWas::Ok:
                    /* Handled above; should never be hit here. */
                    __builtin_trap();
                    
                case ResultWas::ExpressionFailed:
                    if (as.infoMessages.size() == 1) {
                        cause = "expression failed with message";
                    } else if (as.infoMessages.size() > 1) {
                        cause = "expression failed with messages";
                    } else {
                        cause = "expression did not evaluate to true";
                    }
                    break;
                    
                case ResultWas::ExplicitFailure:
                    cause = "failed";
                    break;
                    
                case ResultWas::DidntThrowException:
                    cause = "the expected expression was not thrown";
                    break;
                    
                case ResultWas::ThrewException:
                    cause = "an unexpected exception was thrown";
                    expected = false;
                    break;

                case ResultWas::FatalErrorCondition:
                    cause = "an unexpected error occured";
                    expected = false;
                    break;
                    
                case ResultWas::Info:
                    cause = "info";
                    break;
                    
                case ResultWas::Warning:
                    cause = "warning";
                    break;
                    
                /* Quiesce the compiler; we shouldn't actually hit these */
                case ResultWas::Unknown:
                case ResultWas::FailureBit:
                case ResultWas::Exception:
                    cause = "internal error";
                    expected = false;
                    break;
            }
            
            /* Formulate the error description */
            if (result.hasExpression()) {
                description.append(result.getExpression());
                
                if (result.hasExpandedExpression()) {
                    description.append(" (expands to: " + result.getExpandedExpression() + ")");
                }
            }
            description.append(": " + cause);

            /* Report the base error if there are no additional messages. */
            if (as.infoMessages.size() == 0) {
                auto msg = description;
                if (result.hasMessage()) {
                    msg.append(": " + result.getMessage());
                }
                
                /* Append a terminating period, if necessary */
                if (msg[msg.size() - 1] != '.')
                    msg.append(".");
                
                
                NSString *desc = [NSString stringWithUTF8String: msg.c_str()];
                NSString *file = [NSString stringWithUTF8String: result.getSourceInfo().file.c_str()];
                
                [self recordFailureWithDescription: desc inFile: file atLine: result.getSourceInfo().line expected: expected];
            }
            
            /* Report a failure for each one of the info message strings. */
            for (auto &&info : as.infoMessages) {
                auto msg = description;
                msg.append(": " + info.message);
                
                /* Append a terminating period, if necessary */
                if (msg[msg.size() - 1] != '.')
                    msg.append(".");
                
                NSString *desc = [NSString stringWithUTF8String: msg.c_str()];
                NSString *file = [NSString stringWithUTF8String: info.lineInfo.file.c_str()];
                
                [self recordFailureWithDescription: desc inFile: file atLine: info.lineInfo.line expected: expected];
            }
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
    
const void *XCTestRegistryHub::TestInfoKey = NULL;
const void *XCTestRegistryHub::TestSelectorKey = NULL;

}

@interface XCTestCaseCatchRegistry : NSObject @end
@implementation XCTestCaseCatchRegistry

/* Runs before all C++ initializers; we can insert our custom registry hub here. */
+ (void) load {
    using namespace Catch;
    
    RegistryHub **hub = &getTheRegistryHub();
    RegistryHub *previous = *hub;
    *hub = new XCTestRegistryHub();
    delete previous;
}

@end

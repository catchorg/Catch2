/*
 *  Created by Phil on 18/10/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_TEST_REGISTRY_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_TEST_REGISTRY_HPP_INCLUDED

#include "catch_common.h"
#include "catch_interfaces_testcase.h"
#include "catch_compiler_capabilities.h"
#include "catch_stringref.h"
#include "catch_type_traits.hpp"
#include "catch_preprocessor.hpp"
#include "catch_meta.hpp"

namespace Catch {

template<typename C>
class TestInvokerAsMethod : public ITestInvoker {
    void (C::*m_testAsMethod)();
public:
    TestInvokerAsMethod( void (C::*testAsMethod)() ) noexcept : m_testAsMethod( testAsMethod ) {}

    void invoke() const override {
        C obj;
        (obj.*m_testAsMethod)();
    }
};

auto makeTestInvoker( void(*testAsFunction)() ) noexcept -> ITestInvoker*;

template<typename C>
auto makeTestInvoker( void (C::*testAsMethod)() ) noexcept -> ITestInvoker* {
    return new(std::nothrow) TestInvokerAsMethod<C>( testAsMethod );
}

struct NameAndTags {
    NameAndTags( StringRef const& name_ = StringRef(), StringRef const& tags_ = StringRef() ) noexcept;
    StringRef name;
    StringRef tags;
};

struct AutoReg : NonCopyable {
    AutoReg( ITestInvoker* invoker, SourceLineInfo const& lineInfo, StringRef const& classOrMethod, NameAndTags const& nameAndTags ) noexcept;
    ~AutoReg();
};

} // end namespace Catch

#if defined(CATCH_CONFIG_DISABLE)
    #define INTERNAL_CATCH_TESTCASE_NO_REGISTRATION( TestName, ... ) \
        static void TestName()
    #define INTERNAL_CATCH_TESTCASE_METHOD_NO_REGISTRATION( TestName, ClassName, ... ) \
        namespace{                        \
            struct TestName : INTERNAL_CATCH_REMOVE_PARENS(ClassName) { \
                void test();              \
            };                            \
        }                                 \
        void TestName::test()
    #define INTERNAL_CATCH_TEMPLATE_TEST_CASE_NO_REGISTRATION_2( TestName, TestFunc, Name, Tags, Signature, ... )  \
        INTERNAL_CATCH_DEFINE_SIG_TEST(TestFunc, INTERNAL_CATCH_REMOVE_PARENS(Signature))
    #define INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_NO_REGISTRATION_2( TestNameClass, TestName, ClassName, Name, Tags, Signature, ... )    \
        namespace{                                                                                  \
            namespace INTERNAL_CATCH_MAKE_NAMESPACE(TestName) {                                      \
            INTERNAL_CATCH_DECLARE_SIG_TEST_METHOD(TestName, ClassName, INTERNAL_CATCH_REMOVE_PARENS(Signature));\
        }                                                                                           \
        }                                                                                           \
        INTERNAL_CATCH_DEFINE_SIG_TEST_METHOD(TestName, INTERNAL_CATCH_REMOVE_PARENS(Signature))

    #ifndef CATCH_CONFIG_TRADITIONAL_MSVC_PREPROCESSOR
        #define INTERNAL_CATCH_TEMPLATE_TEST_CASE_NO_REGISTRATION(Name, Tags, ...) \
            INTERNAL_CATCH_TEMPLATE_TEST_CASE_NO_REGISTRATION_2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____F_U_N_C____ ), Name, Tags, typename TestType, __VA_ARGS__ )
    #else
        #define INTERNAL_CATCH_TEMPLATE_TEST_CASE_NO_REGISTRATION(Name, Tags, ...) \
            INTERNAL_CATCH_EXPAND_VARGS( INTERNAL_CATCH_TEMPLATE_TEST_CASE_NO_REGISTRATION_2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____F_U_N_C____ ), Name, Tags, typename TestType, __VA_ARGS__ ) )
    #endif  

    #ifndef CATCH_CONFIG_TRADITIONAL_MSVC_PREPROCESSOR
        #define INTERNAL_CATCH_TEMPLATE_TEST_CASE_SIG_NO_REGISTRATION(Name, Tags, Signature, ...) \
            INTERNAL_CATCH_TEMPLATE_TEST_CASE_NO_REGISTRATION_2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____F_U_N_C____ ), Name, Tags, Signature, __VA_ARGS__ )
    #else
        #define INTERNAL_CATCH_TEMPLATE_TEST_CASE_SIG_NO_REGISTRATION(Name, Tags, Signature, ...) \
            INTERNAL_CATCH_EXPAND_VARGS( INTERNAL_CATCH_TEMPLATE_TEST_CASE_NO_REGISTRATION_2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____F_U_N_C____ ), Name, Tags, Signature, __VA_ARGS__ ) )
    #endif

    #ifndef CATCH_CONFIG_TRADITIONAL_MSVC_PREPROCESSOR
        #define INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_NO_REGISTRATION( ClassName, Name, Tags,... ) \
            INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_NO_REGISTRATION_2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____C_L_A_S_S____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ) , ClassName, Name, Tags, typename T, __VA_ARGS__ )
    #else
        #define INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_NO_REGISTRATION( ClassName, Name, Tags,... ) \
            INTERNAL_CATCH_EXPAND_VARGS( INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_NO_REGISTRATION_2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____C_L_A_S_S____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ) , ClassName, Name, Tags, typename T, __VA_ARGS__ ) )
    #endif

    #ifndef CATCH_CONFIG_TRADITIONAL_MSVC_PREPROCESSOR
        #define INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_SIG_NO_REGISTRATION( ClassName, Name, Tags, Signature, ... ) \
            INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_NO_REGISTRATION_2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____C_L_A_S_S____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ) , ClassName, Name, Tags, Signature, __VA_ARGS__ )
    #else
        #define INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_SIG_NO_REGISTRATION( ClassName, Name, Tags, Signature, ... ) \
            INTERNAL_CATCH_EXPAND_VARGS( INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_NO_REGISTRATION_2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____C_L_A_S_S____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ) , ClassName, Name, Tags, Signature, __VA_ARGS__ ) )
    #endif
#endif

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_TESTCASE2( TestName, ... ) \
        static void TestName(); \
        CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS \
        namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( Catch::makeTestInvoker( &TestName ), CATCH_INTERNAL_LINEINFO, Catch::StringRef(), Catch::NameAndTags{ __VA_ARGS__ } ); } /* NOLINT */ \
        CATCH_INTERNAL_UNSUPPRESS_GLOBALS_WARNINGS \
        static void TestName()
    #define INTERNAL_CATCH_TESTCASE( ... ) \
        INTERNAL_CATCH_TESTCASE2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_S_T____ ), __VA_ARGS__ )

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_METHOD_AS_TEST_CASE( QualifiedMethod, ... ) \
        CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS \
        namespace{ Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( Catch::makeTestInvoker( &QualifiedMethod ), CATCH_INTERNAL_LINEINFO, "&" #QualifiedMethod, Catch::NameAndTags{ __VA_ARGS__ } ); } /* NOLINT */ \
        CATCH_INTERNAL_UNSUPPRESS_GLOBALS_WARNINGS

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_TEST_CASE_METHOD2( TestName, ClassName, ... )\
        CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS \
        namespace{ \
            struct TestName : INTERNAL_CATCH_REMOVE_PARENS(ClassName) { \
                void test(); \
            }; \
            Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar ) ( Catch::makeTestInvoker( &TestName::test ), CATCH_INTERNAL_LINEINFO, #ClassName, Catch::NameAndTags{ __VA_ARGS__ } ); /* NOLINT */ \
        } \
        CATCH_INTERNAL_UNSUPPRESS_GLOBALS_WARNINGS \
        void TestName::test()
    #define INTERNAL_CATCH_TEST_CASE_METHOD( ClassName, ... ) \
        INTERNAL_CATCH_TEST_CASE_METHOD2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_S_T____ ), ClassName, __VA_ARGS__ )

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_REGISTER_TESTCASE( Function, ... ) \
        CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS \
        Catch::AutoReg INTERNAL_CATCH_UNIQUE_NAME( autoRegistrar )( Catch::makeTestInvoker( Function ), CATCH_INTERNAL_LINEINFO, Catch::StringRef(), Catch::NameAndTags{ __VA_ARGS__ } ); /* NOLINT */ \
        CATCH_INTERNAL_UNSUPPRESS_GLOBALS_WARNINGS

    ///////////////////////////////////////////////////////////////////////////////
    #define INTERNAL_CATCH_TEMPLATE_TEST_CASE_2(TestName, TestFunc, Name, Tags, Signature, ... )\
        CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS \
        CATCH_INTERNAL_SUPPRESS_ZERO_VARIADIC_WARNINGS \
        INTERNAL_CATCH_DECLARE_SIG_TEST(TestFunc, INTERNAL_CATCH_REMOVE_PARENS(Signature));\
        namespace {\
        namespace INTERNAL_CATCH_MAKE_NAMESPACE(TestName){\
            INTERNAL_CATCH_TYPE_GEN\
            INTERNAL_CATCH_NTTP_GEN(INTERNAL_CATCH_REMOVE_PARENS(Signature))\
            INTERNAL_CATCH_NTTP_REG_GEN(TestFunc,INTERNAL_CATCH_REMOVE_PARENS(Signature))\
            template<typename...Types> \
            struct TestName{\
                TestName(){\
                    int index = 0;                                    \
                    constexpr char const* tmpl_types[] = {CATCH_REC_LIST(INTERNAL_CATCH_STRINGIZE_WITHOUT_PARENS, __VA_ARGS__)};\
                    using expander = int[];\
                    (void)expander{(reg_test(Types{}, Catch::NameAndTags{ Name " - " + std::string(tmpl_types[index]), Tags } ), index++, 0)... };/* NOLINT */ \
                }\
            };\
            static int INTERNAL_CATCH_UNIQUE_NAME( globalRegistrar ) = [](){\
            TestName<INTERNAL_CATCH_MAKE_TYPE_LISTS_FROM_TYPES(__VA_ARGS__)>();\
            return 0;\
        }();\
        }\
        }\
        CATCH_INTERNAL_UNSUPPRESS_GLOBALS_WARNINGS \
        CATCH_INTERNAL_UNSUPPRESS_ZERO_VARIADIC_WARNINGS \
        INTERNAL_CATCH_DEFINE_SIG_TEST(TestFunc,INTERNAL_CATCH_REMOVE_PARENS(Signature))

#ifndef CATCH_CONFIG_TRADITIONAL_MSVC_PREPROCESSOR
    #define INTERNAL_CATCH_TEMPLATE_TEST_CASE(Name, Tags, ...) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____F_U_N_C____ ), Name, Tags, typename TestType, __VA_ARGS__ )
#else
    #define INTERNAL_CATCH_TEMPLATE_TEST_CASE(Name, Tags, ...) \
        INTERNAL_CATCH_EXPAND_VARGS( INTERNAL_CATCH_TEMPLATE_TEST_CASE_2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____F_U_N_C____ ), Name, Tags, typename TestType, __VA_ARGS__ ) )
#endif  

#ifndef CATCH_CONFIG_TRADITIONAL_MSVC_PREPROCESSOR
    #define INTERNAL_CATCH_TEMPLATE_TEST_CASE_SIG(Name, Tags, Signature, ...) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____F_U_N_C____ ), Name, Tags, Signature, __VA_ARGS__ )
#else
    #define INTERNAL_CATCH_TEMPLATE_TEST_CASE_SIG(Name, Tags, Signature, ...) \
        INTERNAL_CATCH_EXPAND_VARGS( INTERNAL_CATCH_TEMPLATE_TEST_CASE_2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____F_U_N_C____ ), Name, Tags, Signature, __VA_ARGS__ ) )
#endif

    #define INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE2(TestName, TestFuncName, Name, Tags, Signature, TmplTypes, TypesList) \
        CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS                      \
        CATCH_INTERNAL_SUPPRESS_ZERO_VARIADIC_WARNINGS                \
        template<typename TestType> static void TestFuncName();       \
        namespace {\
        namespace INTERNAL_CATCH_MAKE_NAMESPACE(TestName) {                                     \
            INTERNAL_CATCH_TYPE_GEN                                                  \
            INTERNAL_CATCH_NTTP_GEN(INTERNAL_CATCH_REMOVE_PARENS(Signature))         \
            template<typename... Types>                               \
            struct TestName {                                         \
                void reg_tests() {                                          \
                    int index = 0;                                    \
                    using expander = int[];                           \
                    constexpr char const* tmpl_types[] = {CATCH_REC_LIST(INTERNAL_CATCH_STRINGIZE_WITHOUT_PARENS, INTERNAL_CATCH_REMOVE_PARENS(TmplTypes))};\
                    constexpr char const* types_list[] = {CATCH_REC_LIST(INTERNAL_CATCH_STRINGIZE_WITHOUT_PARENS, INTERNAL_CATCH_REMOVE_PARENS(TypesList))};\
                    constexpr auto num_types = sizeof(types_list) / sizeof(types_list[0]);\
                    (void)expander{(Catch::AutoReg( Catch::makeTestInvoker( &TestFuncName<Types> ), CATCH_INTERNAL_LINEINFO, Catch::StringRef(), Catch::NameAndTags{ Name " - " + std::string(tmpl_types[index / num_types]) + "<" + std::string(types_list[index % num_types]) + ">", Tags } ), index++, 0)... };/* NOLINT */\
                }                                                     \
            };                                                        \
            static int INTERNAL_CATCH_UNIQUE_NAME( globalRegistrar ) = [](){ \
                using TestInit = decltype(create<TestName, INTERNAL_CATCH_REMOVE_PARENS(TmplTypes)>(TypeList<INTERNAL_CATCH_MAKE_TYPE_LISTS_FROM_TYPES(INTERNAL_CATCH_REMOVE_PARENS(TypesList))>{})); \
                TestInit t;                                           \
                t.reg_tests();                                        \
                return 0;                                             \
            }();                                                      \
        }                                                             \
        }                                                             \
        CATCH_INTERNAL_UNSUPPRESS_GLOBALS_WARNINGS                    \
        CATCH_INTERNAL_UNSUPPRESS_ZERO_VARIADIC_WARNINGS              \
        template<typename TestType>                                   \
        static void TestFuncName()

#ifndef CATCH_CONFIG_TRADITIONAL_MSVC_PREPROCESSOR
    #define INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE(Name, Tags, ...)\
        INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE2(INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____F_U_N_C____ ), Name, Tags, typename T,__VA_ARGS__)
#else
    #define INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE(Name, Tags, ...)\
        INTERNAL_CATCH_EXPAND_VARGS( INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____F_U_N_C____ ), Name, Tags, typename T, __VA_ARGS__ ) )
#endif

#ifndef CATCH_CONFIG_TRADITIONAL_MSVC_PREPROCESSOR
    #define INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE_SIG(Name, Tags, Signature, ...)\
        INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE2(INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____F_U_N_C____ ), Name, Tags, Signature, __VA_ARGS__)
#else
    #define INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE_SIG(Name, Tags, Signature, ...)\
        INTERNAL_CATCH_EXPAND_VARGS( INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____F_U_N_C____ ), Name, Tags, Signature, __VA_ARGS__ ) )
#endif

    #define INTERNAL_CATCH_TEMPLATE_LIST_TEST_CASE_2(TestName, TestFunc, Name, Tags, TmplList)\
        CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS \
        template<typename TestType> static void TestFunc();       \
        namespace {\
        namespace INTERNAL_CATCH_MAKE_NAMESPACE(TestName){\
        INTERNAL_CATCH_TYPE_GEN\
        template<typename... Types>                               \
        struct TestName {                                         \
            void reg_tests() {                                          \
                int index = 0;                                    \
                using expander = int[];                           \
                (void)expander{(Catch::AutoReg( Catch::makeTestInvoker( &TestFunc<Types> ), CATCH_INTERNAL_LINEINFO, Catch::StringRef(), Catch::NameAndTags{ Name " - " + std::string(INTERNAL_CATCH_STRINGIZE(TmplList)) + " - " + std::to_string(index), Tags } ), index++, 0)... };/* NOLINT */\
            }                                                     \
        };\
        static int INTERNAL_CATCH_UNIQUE_NAME( globalRegistrar ) = [](){ \
                using TestInit = decltype(convert<TestName>(std::declval<TmplList>())); \
                TestInit t;                                           \
                t.reg_tests();                                        \
                return 0;                                             \
            }();                                                        \
        }}\
        CATCH_INTERNAL_UNSUPPRESS_GLOBALS_WARNINGS                    \
        template<typename TestType>                                   \
        static void TestFunc()

    #define INTERNAL_CATCH_TEMPLATE_LIST_TEST_CASE(Name, Tags, TmplList) \
        INTERNAL_CATCH_TEMPLATE_LIST_TEST_CASE_2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____F_U_N_C____ ), Name, Tags, TmplList )


    #define INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_2( TestNameClass, TestName, ClassName, Name, Tags, Signature, ... ) \
        CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS \
        CATCH_INTERNAL_SUPPRESS_ZERO_VARIADIC_WARNINGS \
        namespace {\
        namespace INTERNAL_CATCH_MAKE_NAMESPACE(TestName){ \
            INTERNAL_CATCH_TYPE_GEN\
            INTERNAL_CATCH_NTTP_GEN(INTERNAL_CATCH_REMOVE_PARENS(Signature))\
            INTERNAL_CATCH_DECLARE_SIG_TEST_METHOD(TestName, ClassName, INTERNAL_CATCH_REMOVE_PARENS(Signature));\
            INTERNAL_CATCH_NTTP_REG_METHOD_GEN(TestName, INTERNAL_CATCH_REMOVE_PARENS(Signature))\
            template<typename...Types> \
            struct TestNameClass{\
                TestNameClass(){\
                    int index = 0;                                    \
                    constexpr char const* tmpl_types[] = {CATCH_REC_LIST(INTERNAL_CATCH_STRINGIZE_WITHOUT_PARENS, __VA_ARGS__)};\
                    using expander = int[];\
                    (void)expander{(reg_test(Types{}, #ClassName, Catch::NameAndTags{ Name " - " + std::string(tmpl_types[index]), Tags } ), index++, 0)... };/* NOLINT */ \
                }\
            };\
            static int INTERNAL_CATCH_UNIQUE_NAME( globalRegistrar ) = [](){\
                TestNameClass<INTERNAL_CATCH_MAKE_TYPE_LISTS_FROM_TYPES(__VA_ARGS__)>();\
                return 0;\
        }();\
        }\
        }\
        CATCH_INTERNAL_UNSUPPRESS_GLOBALS_WARNINGS\
        CATCH_INTERNAL_UNSUPPRESS_ZERO_VARIADIC_WARNINGS\
        INTERNAL_CATCH_DEFINE_SIG_TEST_METHOD(TestName, INTERNAL_CATCH_REMOVE_PARENS(Signature))

#ifndef CATCH_CONFIG_TRADITIONAL_MSVC_PREPROCESSOR
    #define INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD( ClassName, Name, Tags,... ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____C_L_A_S_S____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ) , ClassName, Name, Tags, typename T, __VA_ARGS__ )
#else
    #define INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD( ClassName, Name, Tags,... ) \
        INTERNAL_CATCH_EXPAND_VARGS( INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____C_L_A_S_S____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ) , ClassName, Name, Tags, typename T, __VA_ARGS__ ) )
#endif

#ifndef CATCH_CONFIG_TRADITIONAL_MSVC_PREPROCESSOR
    #define INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_SIG( ClassName, Name, Tags, Signature, ... ) \
        INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____C_L_A_S_S____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ) , ClassName, Name, Tags, Signature, __VA_ARGS__ )
#else
    #define INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_SIG( ClassName, Name, Tags, Signature, ... ) \
        INTERNAL_CATCH_EXPAND_VARGS( INTERNAL_CATCH_TEMPLATE_TEST_CASE_METHOD_2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____C_L_A_S_S____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ) , ClassName, Name, Tags, Signature, __VA_ARGS__ ) )
#endif

    #define INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE_METHOD_2(TestNameClass, TestName, ClassName, Name, Tags, Signature, TmplTypes, TypesList)\
        CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS \
        CATCH_INTERNAL_SUPPRESS_ZERO_VARIADIC_WARNINGS \
        template<typename TestType> \
            struct TestName : INTERNAL_CATCH_REMOVE_PARENS(ClassName <TestType>) { \
                void test();\
            };\
        namespace {\
        namespace INTERNAL_CATCH_MAKE_NAMESPACE(TestNameClass) {\
            INTERNAL_CATCH_TYPE_GEN                  \
            INTERNAL_CATCH_NTTP_GEN(INTERNAL_CATCH_REMOVE_PARENS(Signature))\
            template<typename...Types>\
            struct TestNameClass{\
                void reg_tests(){\
                    int index = 0;\
                    using expander = int[];\
                    constexpr char const* tmpl_types[] = {CATCH_REC_LIST(INTERNAL_CATCH_STRINGIZE_WITHOUT_PARENS, INTERNAL_CATCH_REMOVE_PARENS(TmplTypes))};\
                    constexpr char const* types_list[] = {CATCH_REC_LIST(INTERNAL_CATCH_STRINGIZE_WITHOUT_PARENS, INTERNAL_CATCH_REMOVE_PARENS(TypesList))};\
                    constexpr auto num_types = sizeof(types_list) / sizeof(types_list[0]);\
                    (void)expander{(Catch::AutoReg( Catch::makeTestInvoker( &TestName<Types>::test ), CATCH_INTERNAL_LINEINFO, #ClassName, Catch::NameAndTags{ Name " - " + std::string(tmpl_types[index / num_types]) + "<" + std::string(types_list[index % num_types]) + ">", Tags } ), index++, 0)... };/* NOLINT */ \
                }\
            };\
            static int INTERNAL_CATCH_UNIQUE_NAME( globalRegistrar ) = [](){\
                using TestInit = decltype(create<TestNameClass, INTERNAL_CATCH_REMOVE_PARENS(TmplTypes)>(TypeList<INTERNAL_CATCH_MAKE_TYPE_LISTS_FROM_TYPES(INTERNAL_CATCH_REMOVE_PARENS(TypesList))>{}));\
                TestInit t;\
                t.reg_tests();\
                return 0;\
            }(); \
        }\
        }\
        CATCH_INTERNAL_UNSUPPRESS_GLOBALS_WARNINGS \
        CATCH_INTERNAL_UNSUPPRESS_ZERO_VARIADIC_WARNINGS \
        template<typename TestType> \
        void TestName<TestType>::test()

#ifndef CATCH_CONFIG_TRADITIONAL_MSVC_PREPROCESSOR
    #define INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE_METHOD( ClassName, Name, Tags, ... )\
        INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE_METHOD_2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____F_U_N_C____ ), ClassName, Name, Tags, typename T, __VA_ARGS__ )
#else
    #define INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE_METHOD( ClassName, Name, Tags, ... )\
        INTERNAL_CATCH_EXPAND_VARGS( INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE_METHOD_2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____F_U_N_C____ ), ClassName, Name, Tags, typename T,__VA_ARGS__ ) )
#endif

#ifndef CATCH_CONFIG_TRADITIONAL_MSVC_PREPROCESSOR
    #define INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE_METHOD_SIG( ClassName, Name, Tags, Signature, ... )\
        INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE_METHOD_2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____F_U_N_C____ ), ClassName, Name, Tags, Signature, __VA_ARGS__ )
#else
    #define INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE_METHOD_SIG( ClassName, Name, Tags, Signature, ... )\
        INTERNAL_CATCH_EXPAND_VARGS( INTERNAL_CATCH_TEMPLATE_PRODUCT_TEST_CASE_METHOD_2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____F_U_N_C____ ), ClassName, Name, Tags, Signature,__VA_ARGS__ ) )
#endif

    #define INTERNAL_CATCH_TEMPLATE_LIST_TEST_CASE_METHOD_2( TestNameClass, TestName, ClassName, Name, Tags, TmplList) \
        CATCH_INTERNAL_SUPPRESS_GLOBALS_WARNINGS \
        template<typename TestType> \
        struct TestName : INTERNAL_CATCH_REMOVE_PARENS(ClassName <TestType>) { \
            void test();\
        };\
        namespace {\
        namespace INTERNAL_CATCH_MAKE_NAMESPACE(TestName){ \
            INTERNAL_CATCH_TYPE_GEN\
            template<typename...Types>\
            struct TestNameClass{\
                void reg_tests(){\
                    int index = 0;\
                    using expander = int[];\
                    (void)expander{(Catch::AutoReg( Catch::makeTestInvoker( &TestName<Types>::test ), CATCH_INTERNAL_LINEINFO, #ClassName, Catch::NameAndTags{ Name " - " + std::string(INTERNAL_CATCH_STRINGIZE(TmplList)) + " - " + std::to_string(index), Tags } ), index++, 0)... };/* NOLINT */ \
                }\
            };\
            static int INTERNAL_CATCH_UNIQUE_NAME( globalRegistrar ) = [](){\
                using TestInit = decltype(convert<TestNameClass>(std::declval<TmplList>()));\
                TestInit t;\
                t.reg_tests();\
                return 0;\
            }(); \
        }}\
        CATCH_INTERNAL_UNSUPPRESS_GLOBALS_WARNINGS \
        template<typename TestType> \
        void TestName<TestType>::test()

#define INTERNAL_CATCH_TEMPLATE_LIST_TEST_CASE_METHOD(ClassName, Name, Tags, TmplList) \
        INTERNAL_CATCH_TEMPLATE_LIST_TEST_CASE_METHOD_2( INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____ ), INTERNAL_CATCH_UNIQUE_NAME( ____C_A_T_C_H____T_E_M_P_L_A_T_E____T_E_S_T____F_U_N_C____ ), ClassName, Name, Tags, TmplList )


#endif // TWOBLUECUBES_CATCH_TEST_REGISTRY_HPP_INCLUDED

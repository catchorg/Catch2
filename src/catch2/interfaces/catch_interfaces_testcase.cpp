#include <catch2/interfaces/catch_interfaces_testcase.hpp>

namespace Catch {
    ITestInvoker::~ITestInvoker() = default;
    ITestCaseRegistry::~ITestCaseRegistry() = default;
}

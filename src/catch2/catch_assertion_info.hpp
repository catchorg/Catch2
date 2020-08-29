#ifndef CATCH_ASSERTION_INFO_HPP_INCLUDED
#define CATCH_ASSERTION_INFO_HPP_INCLUDED

#include <catch2/internal/catch_result_type.hpp>
#include <catch2/internal/catch_common.hpp>
#include <catch2/internal/catch_stringref.hpp>

namespace Catch {

    struct AssertionInfo {
        // AssertionInfo() = delete;

        StringRef macroName;
        SourceLineInfo lineInfo;
        StringRef capturedExpression;
        ResultDisposition::Flags resultDisposition;
    };

} // end namespace Catch

#endif // CATCH_ASSERTION_INFO_HPP_INCLUDED

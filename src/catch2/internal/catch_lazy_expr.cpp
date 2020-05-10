#include <catch2/internal/catch_lazy_expr.hpp>

#include <catch2/internal/catch_decomposer.hpp>


namespace Catch {

    namespace {
        auto operator <<(std::ostream& os, ITransientExpression const& expr) -> std::ostream& {
            expr.streamReconstructedExpression(os);
            return os;
        }
    }

    auto operator << (std::ostream& os, LazyExpression const& lazyExpr) -> std::ostream& {
        if (lazyExpr.m_isNegated)
            os << "!";

        if (lazyExpr) {
            if (lazyExpr.m_isNegated && lazyExpr.m_transientExpression->isBinaryExpression())
                os << "(" << *lazyExpr.m_transientExpression << ")";
            else
                os << *lazyExpr.m_transientExpression;
        } else {
            os << "{** error - unchecked empty expression requested **}";
        }
        return os;
    }

} // namespace Catch

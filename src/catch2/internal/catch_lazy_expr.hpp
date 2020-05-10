#ifndef CATCH_LAZY_EXPR_HPP_INCLUDED
#define CATCH_LAZY_EXPR_HPP_INCLUDED

#include <iosfwd>

namespace Catch {
        
    struct ITransientExpression;

    class LazyExpression {
        friend class AssertionHandler;
        friend struct AssertionStats;
        friend class RunContext;

        ITransientExpression const* m_transientExpression = nullptr;
        bool m_isNegated;
    public:
        LazyExpression( bool isNegated ):
            m_isNegated(isNegated)
        {}
        LazyExpression(LazyExpression const& other) = default;
        LazyExpression& operator = ( LazyExpression const& ) = delete;

        explicit operator bool() const {
            return m_transientExpression != nullptr;
        }

        friend auto operator << ( std::ostream& os, LazyExpression const& lazyExpr ) -> std::ostream&;
    };

} // namespace Catch

#endif // CATCH_LAZY_EXPR_HPP_INCLUDED

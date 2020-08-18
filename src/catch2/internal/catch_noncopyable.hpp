#ifndef CATCH_NONCOPYABLE_HPP_INCLUDED
#define CATCH_NONCOPYABLE_HPP_INCLUDED

namespace Catch {
    namespace Detail {

        //! Deriving classes become noncopyable and nonmovable
        class NonCopyable {
            NonCopyable( NonCopyable const& ) = delete;
            NonCopyable( NonCopyable&& ) = delete;
            NonCopyable& operator=( NonCopyable const& ) = delete;
            NonCopyable& operator=( NonCopyable&& ) = delete;

        protected:
            NonCopyable() noexcept = default;
        };

    } // namespace Detail
} // namespace Catch

#endif // CATCH_NONCOPYABLE_HPP_INCLUDED

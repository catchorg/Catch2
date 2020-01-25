#include <catch2/catch_matchers_templates.hpp>

namespace Catch {
namespace Matchers {
    namespace Impl {
        std::vector<const void*> vector_cat(std::vector<void const*> const& lhs, std::vector<void const*> const& rhs) {
            auto copy = lhs;
            copy.insert(copy.end(), rhs.begin(), rhs.end());
            return copy;
        }

        std::vector<const void*> vector_cat(std::vector<void const*> const& lhs, void const* rhs) {
            auto copy = lhs;
            copy.push_back(rhs);
            return copy;
        }

        std::vector<const void*> vector_cat(void const* lhs, std::vector<void const*> const& rhs) {
            auto copy = rhs;
            copy.insert(copy.begin(), lhs);
            return copy;
        }
    }
} // namespace Matchers
} // namespace Catch

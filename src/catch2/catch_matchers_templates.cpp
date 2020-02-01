#include <catch2/catch_matchers_templates.hpp>

namespace Catch {
namespace Matchers {
    namespace Impl {
        std::vector<const void*> vector_cat(std::vector<void const*> && lhs, std::vector<void const*> && rhs) {
            lhs.insert(lhs.end(), rhs.begin(), rhs.end());
            return std::move(lhs);
        }

        std::vector<const void*> vector_cat(std::vector<void const*> && lhs, void const* rhs) {
            lhs.push_back(rhs);
            return std::move(lhs);
        }

        std::vector<const void*> vector_cat(void const* lhs, std::vector<void const*> && rhs) {
            rhs.insert(rhs.begin(), lhs);
            return std::move(rhs);
        }
    }
} // namespace Matchers
} // namespace Catch

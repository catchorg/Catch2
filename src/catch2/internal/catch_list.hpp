#ifndef CATCH_LIST_HPP_INCLUDED
#define CATCH_LIST_HPP_INCLUDED

#include <catch2/internal/catch_stringref.hpp>

#include <set>
#include <string>


namespace Catch {

    struct IStreamingReporter;
    class Config;


    struct ReporterDescription {
        std::string name, description;
    };

    struct TagInfo {
        void add(StringRef spelling);
        std::string all() const;

        std::set<StringRef> spellings;
        std::size_t count = 0;
    };

    bool list( IStreamingReporter& reporter, Config const& config );

} // end namespace Catch

#endif // CATCH_LIST_HPP_INCLUDED

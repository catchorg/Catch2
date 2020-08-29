#ifndef CATCH_REPORTER_HELPERS_HPP_INCLUDED
#define CATCH_REPORTER_HELPERS_HPP_INCLUDED

#include <iosfwd>
#include <string>
#include <vector>

namespace Catch {

    struct IConfig;

    // Returns double formatted as %.3f (format expected on output)
    std::string getFormattedDuration( double duration );

    //! Should the reporter show duration of test given current configuration?
    bool shouldShowDuration( IConfig const& config, double duration );

    std::string serializeFilters( std::vector<std::string> const& filters );

    struct lineOfChars {
        char c;
        constexpr lineOfChars( char c ): c( c ) {}

        friend std::ostream& operator<<( std::ostream& out, lineOfChars value );
    };

} // end namespace Catch

#endif // CATCH_REPORTER_HELPERS_HPP_INCLUDED

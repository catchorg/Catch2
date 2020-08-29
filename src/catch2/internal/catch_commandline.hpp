#ifndef CATCH_COMMANDLINE_HPP_INCLUDED
#define CATCH_COMMANDLINE_HPP_INCLUDED

#include <catch2/internal/catch_clara.hpp>

namespace Catch {

    struct ConfigData;

    Clara::Parser makeCommandLineParser( ConfigData& config );

} // end namespace Catch

#endif // CATCH_COMMANDLINE_HPP_INCLUDED

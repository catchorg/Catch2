#ifndef CATCH_INTERFACES_RUNNER_HPP_INCLUDED
#define CATCH_INTERFACES_RUNNER_HPP_INCLUDED

namespace Catch {

    struct IRunner {
        virtual ~IRunner();
        virtual bool aborting() const = 0;
    };
}

#endif // CATCH_INTERFACES_RUNNER_HPP_INCLUDED

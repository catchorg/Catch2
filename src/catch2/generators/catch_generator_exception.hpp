#ifndef CATCH_GENERATOR_EXCEPTION_HPP_INCLUDED
#define CATCH_GENERATOR_EXCEPTION_HPP_INCLUDED

#include <exception>

namespace Catch {

    // Exception type to be thrown when a Generator runs into an error,
    // e.g. it cannot initialize the first return value based on
    // runtime information
    class GeneratorException : public std::exception {
        const char* const m_msg = "";

    public:
        GeneratorException(const char* msg):
            m_msg(msg)
        {}

        const char* what() const noexcept override final;
    };

} // end namespace Catch

#endif // CATCH_GENERATOR_EXCEPTION_HPP_INCLUDED

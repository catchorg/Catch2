// 231-Cfg-OutputStreams.cpp
// Show how to replace the streams with a simple custom made streambuf.

// Note that this reimplementation _does not_ follow `std::cerr`
// semantic, because it buffers the output. For most uses however,
// there is no important difference between having `std::cerr` buffered
// or unbuffered.
#include <catch2/catch_test_macros.hpp>

#include <sstream>
#include <cstdio>

#include <catch2/internal/catch_compiler_detections.hpp>

class out_buff : public std::stringbuf {
    std::FILE* m_stream;
public:
    out_buff(std::FILE* stream):m_stream(stream) {}
    ~out_buff();
    int sync() override {
        int ret = 0;
        for (unsigned char c : str()) {
            if (putc(c, m_stream) == EOF) {
                ret = -1;
                break;
            }
        }
        // Reset the buffer to avoid printing it multiple times
        str("");
        return ret;
    }
};

out_buff::~out_buff() { pubsync(); }

#if defined(CATCH_COMPILER_CLANG)
#pragma clang diagnostic ignored "-Wexit-time-destructors" // static variables in cout/cerr/clog
#endif

namespace Catch {
    std::ostream& cout() {
        static std::ostream ret(new out_buff(stdout));
        return ret;
    }
    std::ostream& clog() {
        static std::ostream ret(new out_buff(stderr));
        return ret;
    }
    std::ostream& cerr() {
        return clog();
    }
}


TEST_CASE("This binary uses putc to write out output", "[compilation-only]") {
    SUCCEED("Nothing to test.");
}

//License: Boost 1.0
//By Paul Dreik 2020

#include "internal/catch_xmlwriter.hpp"

#include <iostream>

// from https://stackoverflow.com/a/8244052
class NulStreambuf : public std::streambuf {
  char dummyBuffer[64];

protected:
  virtual int overflow(int c) override final;
};
int NulStreambuf::overflow(int c){
    setp(dummyBuffer, dummyBuffer + sizeof(dummyBuffer));
    return (c == traits_type::eof()) ? '\0' : c;
}
class NulOStream final : private NulStreambuf, public std::ostream {
public:
  NulOStream() : std::ostream(this) {}
  NulStreambuf *rdbuf() { return this; }
  virtual void avoidOutOfLineVirtualCompilerWarning();
};

void NulOStream::avoidOutOfLineVirtualCompilerWarning()
{
}


extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {

    std::string buf(Data,Data+Size);
    NulOStream nul;
    Catch::XmlEncode encode(buf);
    encode.encodeTo(nul);
    return 0;
}



#include "NullOStream.h"

void NullOStream::avoidOutOfLineVirtualCompilerWarning()
{
}

int NullStreambuf::overflow(int c){
    setp(dummyBuffer, dummyBuffer + sizeof(dummyBuffer));
    return (c == traits_type::eof()) ? '\0' : c;
}

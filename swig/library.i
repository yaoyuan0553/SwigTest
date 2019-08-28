%include "std_string.i"
//%include "arrays_java.i"
%include "carrays.i"

%module library
%{
#include "library.h"
%}

%apply unsigned int { uint32_t };
%apply unsigned long { uint64_t };

%include "library.h"

//%apply char[] {char*};

%array_class(int, IntArray);

%template(PairII) Pair<int, int>;
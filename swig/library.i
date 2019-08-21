%module library
%{
#include "library.h"
%}

%include "std_string.i"
%include "arrays_java.i"
%include "library.h"

//%apply char[] {char*};

%template(PairII) Pair<int, int>;
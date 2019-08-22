%module library
%{
#include "library.h"
%}

%include "std_string.i"
//%include "arrays_java.i"
%include "carrays.i"
%include "library.h"

//%apply char[] {char*};

%array_class(int, IntArray);

%template(PairII) Pair<int, int>;
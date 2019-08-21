%module library
%{
#include "library.h"
%}

%include "library.h"
%include "std_string.i"

%template(PairII) Pair<int, int>;
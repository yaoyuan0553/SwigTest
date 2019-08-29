%define DOCSTRING
"This is a SWIG-generated python wrapper for C++ database query \n"
"functionality, including APIs to extract various info from \n"
"generated database binary files by DatabaseGenerator"
%enddef

%include "std_string.i"
%include "std_vector.i"
%include "std_shared_ptr.i"
%include "carrays.i"
%include "cpointer.i"
%include "typemaps.i"

%shared_ptr(IdDataRecord);

/* make IndexValue a read-only class */
%immutable IndexValue::pid;
%immutable IndexValue::aid;
%immutable IndexValue::appDate;
%immutable IndexValue::ipc;
%immutable IndexValue::binId;
%immutable IndexValue::ti;
%immutable IndexValue::ai;
%immutable IndexValue::ci;
%immutable IndexValue::di;
%immutable IndexValue::offset;

%module(docstring=DOCSTRING) library
%{
#include "library.h"
#include "ConcurrentStaticQueue.h"
%}

%apply unsigned int { uint32_t };
%apply unsigned long { uint64_t };

namespace std {
    %template(IntVector) vector<int>;

    %template(StringVector) vector<string>;

    %template(IndexValueVector) vector<IndexValue*>;

    %template(IdDataRecordVector) std::vector<std::shared_ptr<IdDataRecord>>;
};

%include "library.h"
%include "DataBasicTypes.h"
%include "ConcurrentStaticQueue.h"

%array_class(int, IntArray);

%template(PairII) Pair<int, int>;
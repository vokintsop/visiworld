/* File: streetglass.i */

%module(directors="1") streetglass

/* Generate director code for the following methods
%feature("director");*/

/* Support std::string to JAVA */
%include std_string.i

/* Support std::vector to JAVA */
%include std_vector.i

/* Includes in the generated JNI c file */
%{
#include "streetglass.h"
%}

%ignore operator=;
%ignore operator<;
%ignore operator==;

/* Take the classes from this file and generate the JAVA/JNI bindings */
%include "streetglass.h"

namespace std {
   %template(StringVector) vector<string>;
}

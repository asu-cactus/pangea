
#ifndef RECORD_H
#define RECORD_H

#include <cstddef>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cstring>

namespace pdb {

template <class ObjType>
class Handle;

#define CHAR_PTR(c) ((char*)c)

// This is a raw, bitwise representation of an object.  It is created
// using the getRecord () function, and then a handle can be extracted
// from an object using the getRootObject () function.  For example,
// here is some sample code that uses the Record class:
//
// /* Create an object */
//
// Handle <Supervisor> foo = makeObject <Supervisor> (134, 124.5, myEmp);
//
// /* Use getRecord to get the raw bytes of the object */
//
// Record <Supervisor> *myBytes = getRecord <Supervisor> (foo);
//
// /* Write the object to a file */
//
// int filedesc = open ("testfile", O_WRONLY | O_APPEND);
// write (filedesc, myBytes, myBytes->numBytes);
// close (filedesc);
//
// /* Then, at a later time, get the object back */
//
// filedesc = open ("testfile", O_RDONLY);
// Record <Supervisor> *myNewBytes = (Record <Supervisor> *) malloc (myBytes->numBytes);
// read (filedesc, myNewBytes, myBytes->numBytes);
// Handle <Supervisor> bar = myNewBytes->getRootObject ();
//
// Note that ObjType is the type of the root object in the record
//
template <class ObjType>
class Record {

public:
    // this returns the number of bytes that need to be moved around
    size_t numBytes();

    // the location of the root object in the Record
    size_t rootObjectOffset();

    // get a handle to the root object
    Handle<ObjType> getRootObject();
};
}

#include "Record.cc"

#endif

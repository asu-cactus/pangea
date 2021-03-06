
#ifndef REF_COUNTED_OBJECT_CC
#define REF_COUNTED_OBJECT_CC

#include <cstddef>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <climits>
#include <cstring>

#include "Allocator.h"
#include "RefCountedObject.h"
#include "RefCountMacros.h"

namespace pdb {

template <class ObjType>
void RefCountedObject<ObjType>::setRefCount(unsigned toMe) {

    // don't change the ref count for an un-managed object
    if (!getAllocator().isManaged(this)) {
        return;
    }

    NUM_COPIES = toMe;
}

template <class ObjType>
unsigned RefCountedObject<ObjType>::getRefCount() {

    // an un-managed ref count is always huge
    if (!getAllocator().isManaged(this)) {
        return UINT_MAX;
    }

    return NUM_COPIES;
}

template <class ObjType>
void RefCountedObject<ObjType>::incRefCount() {

    // don't change the ref count for an un-managed object
    if (!getAllocator().isManaged(this)) {
        return;
    }

    NUM_COPIES++;
}

// remember the ref count
template <class ObjType>
void RefCountedObject<ObjType>::decRefCount(PDBTemplateBase& typeInfo) {

    // don't change the ref count for an un-managed object
    if (!getAllocator().isManaged(this)) {
        return;
    }

    NUM_COPIES--;

    // if the ref count goes to zero, free the pointed-to object
    if (NUM_COPIES == 0) {
        if (typeInfo.getTypeCode() == 0) {
            PDB_COUT << "RefCountedObject::decRefCount: typeInfo=0 before deleteConstituentObject"
                     << std::endl;
        }
        // call the appropriate delete to recursively free, if needed
        typeInfo.deleteConstituentObject(getObject());

// and free the RAM
#ifdef DEBUG_OBJECT_MODEL
        getAllocator().freeRAM(CHAR_PTR(this), typeInfo.getTypeCode());
#else
        getAllocator().freeRAM(CHAR_PTR(this));
#endif
    }
}

template <class ObjType>
ObjType* RefCountedObject<ObjType>::getObject() const {
    return (ObjType*)(CHAR_PTR(this) + REF_COUNT_PREAMBLE_SIZE);
}
}

#endif

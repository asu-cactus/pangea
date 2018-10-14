
#ifndef SET_SCAN_H
#define SET_SCAN_H

#include "Object.h"
#include "Handle.h"
#include "QueryBase.h"
#include "PDBString.h"

// PRELOAD %SetScan%

namespace pdb {

// encapsulates a request to scan a set stored in the database
class SetScan : public Object {

public:
    SetScan(std::string dbNameIn, std::string setNameIn) {
        dbName = dbNameIn;
        setName = setNameIn;
    }

    SetScan() {}
    ~SetScan() {}

    String getDatabase() {
        return dbName;
    }

    String getSetName() {
        return setName;
    }

    ENABLE_DEEP_COPY

private:
    // this is the database that we are computing over
    String dbName;

    // and the set
    String setName;
};
}

#endif

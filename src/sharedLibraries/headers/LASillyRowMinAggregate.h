#ifndef SILLY_LA_ROW_MIN_AGGREGATE_H
#define SILLY_LA_ROW_MIN_AGGREGATE_H

// by Binhang, May 2017

#include "ClusterAggregateComp.h"
#include "MatrixBlock.h"
#include "LambdaCreationFunctions.h"


using namespace pdb;

class LASillyRowMinAggregate
    : public ClusterAggregateComp<MatrixBlock, MatrixBlock, MatrixMeta, MatrixData> {

public:
    ENABLE_DEEP_COPY

    LASillyRowMinAggregate() {}

    // the key type must have == and size_t hash () defined
    Lambda<MatrixMeta> getKeyProjection(Handle<MatrixBlock> aggMe) override {
        return makeLambdaFromMethod(aggMe, getRowKey);
    }

    // the value type must have + defined
    Lambda<MatrixData> getValueProjection(Handle<MatrixBlock> aggMe) override {
        return makeLambdaFromMethod(aggMe, getRowMinValue);
    }
};


#endif
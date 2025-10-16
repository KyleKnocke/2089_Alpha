#pragma once

#include "CoreMinimal.h"
#include "WorldGenTypes.h"

/** Simple LRU cache for generated sectors. */
class WORLDGEN_API FSectorCache
{
public:
    FSectorCache();

    void SetCapacity(int32 InCapacity);

    bool Get(const FIntPoint& Key, FSectorDescriptor& OutDescriptor) const;

    void Put(const FSectorDescriptor& Descriptor);

    void Clear();

private:
    mutable TMap<FIntPoint, FSectorDescriptor> Storage;
    mutable TArray<FIntPoint> Order;
    int32 Capacity;
};

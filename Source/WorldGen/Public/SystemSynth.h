#pragma once

#include "CoreMinimal.h"
#include "WorldGenTypes.h"

class FAstroPCG64;

namespace WorldGen
{
    /** Synthesizes a deterministic star record for the provided address. */
    WORLDGEN_API FStarRecord MakeStarRecord(const FStarAddress& Address, int64 MasterSeed);

    /** Generates a deterministic system descriptor. */
    WORLDGEN_API FSystemDescriptor MakeSystem(const FStarAddress& Address, int64 MasterSeed);
}

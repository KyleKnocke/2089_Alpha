#pragma once

#include "CoreMinimal.h"

/**
 * Simple PCG64 implementation for deterministic galaxy generation.
 */
class SIMASTRO_API FAstroPCG64
{
public:
    FAstroPCG64();
    explicit FAstroPCG64(uint64 Seed);

    /** Seeds the generator. */
    void Initialize(uint64 Seed);

    /** Returns a 64-bit random value. */
    uint64 Next64();

    /** Returns a double in [0,1). */
    double NextDouble();

    /** Returns an integer within [0, MaxExclusive). */
    int32 NextInt(const int32 MaxExclusive);

private:
    uint64 State;
    uint64 Increment;
};

/** Deterministic hash of multiple 64-bit values. */
SIMASTRO_API uint64 AstroHash(uint64 Seed, uint64 A, uint64 B = 0ull, uint64 C = 0ull);

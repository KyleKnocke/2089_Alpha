#include "AstroRNG.h"

namespace
{
    constexpr uint64 DefaultIncrement = 1442695040888963407ull;
}

FAstroPCG64::FAstroPCG64()
    : State(0), Increment(DefaultIncrement)
{
}

FAstroPCG64::FAstroPCG64(const uint64 Seed)
    : FAstroPCG64()
{
    Initialize(Seed);
}

void FAstroPCG64::Initialize(const uint64 Seed)
{
    State = 0u;
    Increment = (Seed << 1u) | 1u;
    Next64();
    State += Seed;
    Next64();
}

uint64 FAstroPCG64::Next64()
{
    const uint64 OldState = State;
    State = OldState * 6364136223846793005ull + Increment;
    const uint64 XorShifted = ((OldState >> 18u) ^ OldState) >> 27u;
    const uint64 Rot = OldState >> 59u;
    return (XorShifted >> Rot) | (XorShifted << ((-Rot) & 31));
}

double FAstroPCG64::NextDouble()
{
    const uint64 Value = Next64() >> 11;
    return static_cast<double>(Value) * 0x1.0p-53;
}

int32 FAstroPCG64::NextInt(const int32 MaxExclusive)
{
    if (MaxExclusive <= 0)
    {
        return 0;
    }

    const uint64 Threshold = (0u - static_cast<uint64>(MaxExclusive)) % MaxExclusive;
    while (true)
    {
        const uint64 R = Next64();
        if (R >= Threshold)
        {
            return static_cast<int32>(R % MaxExclusive);
        }
    }
}

uint64 AstroHash(const uint64 Seed, const uint64 A, const uint64 B, const uint64 C)
{
    uint64 Hash = Seed * 11400714819323198485ull + 0x9E3779B97F4A7C15ull;
    Hash ^= A + 0x9E3779B97F4A7C15ull + (Hash << 6) + (Hash >> 2);
    Hash ^= B + 0x9E3779B97F4A7C15ull + (Hash << 6) + (Hash >> 2);
    Hash ^= C + 0x9E3779B97F4A7C15ull + (Hash << 6) + (Hash >> 2);
    return Hash;
}

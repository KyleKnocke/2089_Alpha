#include "AstroRNG.h"

#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAstroRngDeterminismTest, "SimAstro.RNG.Determinism", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAstroRngDeterminismTest::RunTest(const FString& Parameters)
{
    FAstroPCG64 RngA(1234);
    FAstroPCG64 RngB(1234);

    for (int32 Index = 0; Index < 100; ++Index)
    {
        const uint64 ValueA = RngA.Next64();
        const uint64 ValueB = RngB.Next64();
        if (ValueA != ValueB)
        {
            AddError(TEXT("PCG generator produced different values for same seed."));
            return false;
        }
    }

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS

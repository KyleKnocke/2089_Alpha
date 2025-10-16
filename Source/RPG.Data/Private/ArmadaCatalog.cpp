#include "ArmadaCatalog.h"

#include "AstroRNG.h"

UArmadaCatalog::UArmadaCatalog()
{
    Factions =
    {
        {TEXT("Helios Nomads"), TEXT("Trade convoys that chain micro-jumps"), TEXT("Industry"), 1001, {3, 8}, FIntPoint(12, -3), 15},
        {TEXT("Aegis Veil"), TEXT("Stealth interdiction and recon"), TEXT("Cyber"), 2027, {4, 12}, FIntPoint(-5, 6), 12},
        {TEXT("Argent Tide"), TEXT("Rapid relief flotillas"), TEXT("Bio"), 3188, {5, 14}, FIntPoint(0, 0), 10},
        {TEXT("Vulcan Cartel"), TEXT("Resource raiders"), TEXT("Industry"), 4512, {6, 16}, FIntPoint(30, -15), 18}
    };
}

#include "SectorCache.h"

FSectorCache::FSectorCache()
    : Capacity(32)
{
}

void FSectorCache::SetCapacity(const int32 InCapacity)
{
    Capacity = FMath::Max(1, InCapacity);
    while (Order.Num() > Capacity)
    {
        const FIntPoint Oldest = Order[0];
        Order.RemoveAt(0);
        Storage.Remove(Oldest);
    }
}

bool FSectorCache::Get(const FIntPoint& Key, FSectorDescriptor& OutDescriptor) const
{
    if (const FSectorDescriptor* Found = Storage.Find(Key))
    {
        OutDescriptor = *Found;
        Order.Remove(Key);
        Order.Add(Key);
        return true;
    }
    return false;
}

void FSectorCache::Put(const FSectorDescriptor& Descriptor)
{
    Storage.Add(Descriptor.Sector, Descriptor);
    Order.Remove(Descriptor.Sector);
    Order.Add(Descriptor.Sector);

    while (Order.Num() > Capacity)
    {
        const FIntPoint Oldest = Order[0];
        Order.RemoveAt(0);
        Storage.Remove(Oldest);
    }
}

void FSectorCache::Clear()
{
    Storage.Reset();
    Order.Reset();
}

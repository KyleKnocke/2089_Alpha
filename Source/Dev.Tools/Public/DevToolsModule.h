#pragma once

#include "Modules/ModuleManager.h"

class FDevToolsModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};


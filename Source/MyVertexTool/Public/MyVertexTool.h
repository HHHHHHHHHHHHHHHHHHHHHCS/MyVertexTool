#pragma once

#include "Modules/ModuleManager.h"

class FMyVertexToolModule : public IModuleInterface
{
public:

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

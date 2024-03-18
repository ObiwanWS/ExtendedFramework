// Copyright (c) 2024 Obiwan Medeiros.

#pragma once

#include <Modules/ModuleInterface.h>

class FExtendedGameFrameworkModule : public IModuleInterface
{
public:
	//~ Begin of IModuleInterface implementation.
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~ End of IModuleInterface implementation.
};

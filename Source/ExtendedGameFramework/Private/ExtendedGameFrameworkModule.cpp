// Copyright (c) 2024 Obiwan Medeiros.

#include "ExtendedGameFrameworkModule.h"

#include <Modules/ModuleManager.h>

#define LOCTEXT_NAMESPACE "FExtendedGameFrameworkModule"

void FExtendedGameFrameworkModule::StartupModule()
{
	// This code will execute after your module is loaded into memory.
}

void FExtendedGameFrameworkModule::ShutdownModule()
{
	// This function may be called during shutdown or unloading to clean up your module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FExtendedGameFrameworkModule, ExtendedGameFramework)

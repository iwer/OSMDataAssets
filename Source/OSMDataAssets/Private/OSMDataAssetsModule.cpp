// Copyright (c) Iwer Petersen. All rights reserved.

#include "OSMDataAssetsModule.h"

#define LOCTEXT_NAMESPACE "FOSMDataAssetsModule"

void FOSMDataAssetsModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FOSMDataAssetsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOSMDataAssetsModule, OSMDataAssets)
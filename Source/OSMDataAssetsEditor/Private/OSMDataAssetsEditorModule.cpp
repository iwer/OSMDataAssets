// Copyright (c) Iwer Petersen. All rights reserved.
#include "Modules/ModuleManager.h"
#include "Toolkits/AssetEditorToolkit.h"


#define LOCTEXT_NAMESPACE "FOSMDataAssetsEditorModule"

class FOSMDataAssetsEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override
	{
		RegisterAssetTools();
    }

	virtual void ShutdownModule() override
	{
        UnregisterAssetTools();
	}

    virtual bool SupportsDynamicReloading() override
	{
		return true;
	}
protected:
    /** Registers asset tool actions. */
    void RegisterAssetTools()
    {
    }



    /** Unregisters asset tool actions. */
	void UnregisterAssetTools()
	{

	}


private:

};

IMPLEMENT_MODULE(FOSMDataAssetsEditorModule, OSMDataAssetsEditor);
#undef LOCTEXT_NAMESPACE

// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class FMyVertexToolEditorModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** This function will be bound to Command. */
	void PluginButtonClicked();

private:
	void RegisterMenus();

	TSharedRef<SWidget> CreateComboToolContent() const;

	static void OnMenuClicked_VertexTool();
	
private:
	TSharedPtr<class FUICommandList> PluginCommands;
};

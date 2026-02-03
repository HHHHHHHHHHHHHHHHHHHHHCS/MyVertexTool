// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyVertexToolEditor.h"
#include "MyVertexToolEditorStyle.h"
#include "MyVertexToolEditorCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

static const FName MyVertexToolTabName("MyVertexTool");

#define LOCTEXT_NAMESPACE "FMyVertexToolModule"

void FMyVertexToolEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FMyVertexToolEditorStyle::Initialize();
	FMyVertexToolEditorStyle::ReloadTextures();

	FMyVertexToolEditorCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FMyVertexToolEditorCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FMyVertexToolEditorModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMyVertexToolEditorModule::RegisterMenus));
}

void FMyVertexToolEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FMyVertexToolEditorStyle::Shutdown();

	FMyVertexToolEditorCommands::Unregister();
}

void FMyVertexToolEditorModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	FText DialogText = FText::Format(
		LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
		FText::FromString(TEXT("FMyVertexToolModule::PluginButtonClicked()")),
		FText::FromString(TEXT("MyVertexTool.cpp"))
	);
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}

void FMyVertexToolEditorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FMyVertexToolEditorCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FMyVertexToolEditorCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMyVertexToolEditorModule, MyVertexToolEditor)

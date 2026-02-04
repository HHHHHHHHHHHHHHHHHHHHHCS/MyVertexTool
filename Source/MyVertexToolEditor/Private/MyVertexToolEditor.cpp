// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyVertexToolEditor.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "MyVertexToolEditorStyle.h"
#include "MyVertexToolEditorCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

static const FName MyVertexToolEditorTabName("MyVertexToolEditor");

#define LOCTEXT_NAMESPACE "FMyVertexToolEditorModule"

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
		FText::FromString(TEXT("FMyVertexToolEditorModule::PluginButtonClicked()")),
		FText::FromString(TEXT("MyVertexToolEditor.cpp"))
	);
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}

void FMyVertexToolEditorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	//扩展菜单项
	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
	if (!ToolbarMenu)
	{
		return;
	}

	// UE 如果要多个插件使用一个ToolbarMenu, 需要再创建一个单独的插件来创建合集的UI
	FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("MyEditorToolsMenu");

	// 如何找到 EditorViewportToolBar.OptionsDropdown 对应的 UI 图标
	// 可以看 Engine/Source/Editor/EditorStyle/Private/StarshipStyle.cpp 查找 EditorViewportToolBar.OptionsDropdown
	// 打开 Engine/Content/Editor/Slate 文件夹, 再根据 IMAGE_BRUSH_SVG("Starship/EditorViewport/menu") 就是对应的UI
	const FUIAction PlatformMenuShownDelegate;
	FToolMenuEntry Entry = FToolMenuEntry::InitComboButton(
		"MyEditorToolsMenu",
		PlatformMenuShownDelegate,
		FOnGetContent::CreateLambda([this] { return CreateComboToolContent(); }),
		LOCTEXT("MyEditorTools", "MyEditorTools"),
		LOCTEXT("MyEditorTools_Tooltip", "MyEditorTools_Tooltip"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "EditorViewportToolBar.OptionsDropdown"),
		false,
		"PlatformsMenu");
	Entry.StyleNameOverride = "CalloutToolbar";
	Section.AddEntry(Entry);
}

TSharedRef<SWidget> FMyVertexToolEditorModule::CreateComboToolContent() const
{
	static const FName MenuName("LevelEditor.MainMenu.Window.MyEditorToolsMenu");
	if (!UToolMenus::Get()->IsMenuRegistered(MenuName))
	{
		UToolMenu* Menu = UToolMenus::Get()->RegisterMenu(MenuName);
		// 可以添加不同的selection, 分割用
		// FToolMenuSection& ManagePlatformsSection = Menu->AddSection("MyToolsMenu_Other", LOCTEXT("MyEditorTools_Other", "MyEditorTools/Other"));
		FToolMenuSection& ManagePlatformsSection = Menu->AddSection("MyToolsMenu", LOCTEXT("MyEditorTools", "MyEditorTools"));
		ManagePlatformsSection.AddDynamicEntry(
			FName("MyEditorTools"),
			FNewToolMenuSectionDelegate::CreateLambda([](FToolMenuSection& PlatformsSection)
			{
				PlatformsSection.AddMenuEntry(
					NAME_None,
					LOCTEXT("VertexToolMenu", "VertexToolMenu"),
					LOCTEXT("VertexToolMenu_Tooltip", "VertexToolMenu"),
					FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Viewports"),
					FUIAction(
						FExecuteAction::CreateStatic(&FMyVertexToolEditorModule::OnMenuClicked_VertexTool))
				);

				// 如果是UI合集插件, 这里可以继续添加按钮
			}));
	}

	const FToolMenuContext MenuContext(PluginCommands);

	return UToolMenus::Get()->GenerateWidget(MenuName, MenuContext);
}

void FMyVertexToolEditorModule::OnMenuClicked_VertexTool()
{
	UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
	if (!EditorUtilitySubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get EditorUtilitySubsystem"));
		return;
	}
	const FString BlueprintPathStr = TEXT("/Script/Blutility.EditorUtilityWidgetBlueprint'/MyVertexTool/Editor/Blueprints/EUW_MyVertexToolEditor.EUW_MyVertexToolEditor'");
	const FSoftObjectPath BlueprintPath = BlueprintPathStr;
	UObject* BlueprintObject = BlueprintPath.TryLoad();
	UEditorUtilityWidgetBlueprint* LoadedEditorUtilityBlueprint = Cast<UEditorUtilityWidgetBlueprint>(BlueprintObject);
	EditorUtilitySubsystem->SpawnAndRegisterTabWithId(LoadedEditorUtilityBlueprint, FName(TEXT("MyVertexTool")));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMyVertexToolEditorModule, MyVertexToolEditor)

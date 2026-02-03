// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyVertexToolEditorStyle.h"
#include "MyVertexToolEditor.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FMyVertexToolEditorStyle::StyleInstance = nullptr;

void FMyVertexToolEditorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FMyVertexToolEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FMyVertexToolEditorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("MyVertexToolEditorStyle"));
	return StyleSetName;
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef<FSlateStyleSet> FMyVertexToolEditorStyle::Create()
{
	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet("MyVertexToolEditorStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("MyVertexTool")->GetBaseDir() / TEXT("Resources"));

	Style->Set("MyVertexTool.PluginAction", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));
	return Style;
}

void FMyVertexToolEditorStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FMyVertexToolEditorStyle::Get()
{
	return *StyleInstance;
}

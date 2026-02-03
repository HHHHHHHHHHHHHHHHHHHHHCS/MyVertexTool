// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyVertexToolEditorCommands.h"

#define LOCTEXT_NAMESPACE "FMyVertexToolModule"

void FMyVertexToolEditorCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "MyVertexTool", "Execute MyVertexTool action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE

// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"
#include "MyVertexToolEditorStyle.h"

class FMyVertexToolEditorCommands : public TCommands<FMyVertexToolEditorCommands>
{
public:
	FMyVertexToolEditorCommands()
		: TCommands<FMyVertexToolEditorCommands>(
			TEXT("MyVertexToolEditor"),
			NSLOCTEXT("Contexts", "MyVertexToolEditor", "MyVertexToolEditor Plugin"),
			NAME_None,
			FMyVertexToolEditorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> PluginAction;
};

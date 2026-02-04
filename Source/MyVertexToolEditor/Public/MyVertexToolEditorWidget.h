#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"

#include "MyVertexToolEditorWidget.generated.h"

class UEditorUtilityButton;

UCLASS()
class MYVERTEXTOOLEDITOR_API UMyVertexToolEditorWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UEditorUtilityButton* Button_CreateMyMesh;

public:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnClick_CreateMyMesh();

	static bool OpenSaveAssetDialog(FString& outPackagePath, FString& outAssetName);
	void CreateMyMesh(FString packagePath, FString assetName);
};

#include "MyVertexToolEditorWidget.h"

void UMyVertexToolEditorWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMyVertexToolEditorWidget::OnClick_CreateMyMesh()
{
}

bool UMyVertexToolEditorWidget::OpenSaveAssetDialog(FString& outPackagePath, FString& outAssetName)
{
	return true;
}

void UMyVertexToolEditorWidget::CreateMyMesh(FString packagePath, FString assetName)
{
}

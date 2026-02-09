#include "MyVertexToolEditorWidget.h"
#include "DesktopPlatformModule.h"
#include "EditorUtilityWidgetComponents.h"
#include "StaticMeshAttributes.h"
#include "AssetRegistry/AssetRegistryModule.h"

void UMyVertexToolEditorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(Button_CreateMyMesh))
	{
		Button_CreateMyMesh->OnClicked.AddDynamic(this, &UMyVertexToolEditorWidget::OnClick_CreateMyMesh);
	}
}

void UMyVertexToolEditorWidget::OnClick_CreateMyMesh()
{
	FString outPackagePath;
	FString outAssetName;
	bool canSave = OpenSaveAssetDialog(outPackagePath, outAssetName);

	if (!canSave)
	{
		return;
	}

	CreateMyMesh(outPackagePath, outAssetName);
}

bool UMyVertexToolEditorWidget::OpenSaveAssetDialog(FString& outPackagePath, FString& outAssetName)
{
	IDesktopPlatform* desktopPlatform = FDesktopPlatformModule::Get();
	if (!desktopPlatform)
	{
		return false;
	}

	void* parentWindowHandle = nullptr;

	const FString defaultPath = FPaths::ProjectContentDir();
	const FString defaultFile = TEXT("MyMeshAsset");

	TArray<FString> outFiles;

	bool bResult = desktopPlatform->SaveFileDialog(
		parentWindowHandle,
		TEXT("Save Asset"),
		defaultPath,
		defaultFile,
		TEXT("Unreal Asset (*.uasset)|*.uasset"),
		EFileDialogFlags::None,
		outFiles
	);

	if (!bResult || outFiles.Num() == 0)
	{
		return false;
	}

	FString fullPath = outFiles[0];

	// 必须在 Content 目录下
	if (!fullPath.StartsWith(FPaths::ProjectContentDir()))
	{
		UE_LOG(LogTemp, Error, TEXT("Asset must be saved under Content folder"));
		return false;
	}

	// 磁盘路径 -> /Game/ 路径
	FString relativePath = fullPath;
	FPaths::MakePathRelativeTo(relativePath, *FPaths::ProjectContentDir());

	FString packagePath = TEXT("/Game/") + FPaths::GetPath(relativePath);
	FString assetName = FPaths::GetBaseFilename(relativePath);

	outPackagePath = packagePath;
	outAssetName = assetName;
	return true;
}

void UMyVertexToolEditorWidget::CreateMyMesh(FString packagePath, FString assetName)
{
	constexpr float k_MeshHeight = 4.0f;
	constexpr float k_MeshRadius = 1.0f;
	constexpr int32 k_MeshWNum = 30;
	constexpr int32 k_MeshHNum = 13;

	FVector3f vertices[k_MeshWNum + 1][k_MeshHNum * 2 + 1];
	FVector2f uvs[k_MeshWNum + 1][k_MeshHNum * 2 + 1];
	float delta = 2.0f * PI / k_MeshWNum;
	for (int32 i = 0; i < k_MeshWNum + 1; i++)
	{
		float a = i * delta;
		if (i == k_MeshWNum)
		{
			a = 0;
		}

		float x = FMath::Sin(a);
		float z = FMath::Cos(a);
		for (int j = -k_MeshHNum; j <= k_MeshHNum; j++)
		{
			float y = k_MeshHeight * j / k_MeshHNum;

			float r = (1 - (float)FMath::Abs(j) / k_MeshHNum) * k_MeshRadius;
			// 注意向上的轴是z, 尺寸单位是 cm 所以x100
			vertices[i][j + k_MeshHNum] = 100.0f * FVector3f(x * r, z * r, y);

			FVector2f uv = FVector2f(
				4.0f * i / k_MeshWNum,
				(float)j / k_MeshHNum);

			// 因为UE UV Y 和 Unity 不太一样
			uv.Y = -uv.Y;

			uvs[i][j + k_MeshHNum] = uv;
		}
	}

	constexpr int32 k_VertexCount = (k_MeshWNum + 1) * (2 * k_MeshHNum + 1);
	constexpr int32 k_IndicesCount = k_MeshWNum * (2 * k_MeshHNum) * 6;

	// 把二维数组转换为一维线性数据
	TArray<FVector3f> vertArray;
	vertArray.SetNum(k_VertexCount);
	TArray<FVector2f> uvArray;
	uvArray.SetNum(k_VertexCount);
	TArray<int32> indexArray;
	indexArray.SetNum(k_IndicesCount);

	int32 currIdx = 0;
	for (int32 i = 0; i < k_MeshWNum + 1; i++)
	{
		for (int32 j = 0; j < k_MeshHNum * 2 + 1; j++)
		{
			int32 idx = i * (k_MeshHNum * 2 + 1) + j;
			vertArray[idx] = vertices[i][j];
			uvArray[idx] = uvs[i][j];
			currIdx++;
		}
	}

	currIdx = 0;
	for (int32 i = 0; i < k_MeshWNum; i++)
	{
		for (int32 j = 0; j < k_MeshHNum * 2; j++)
		{
			// Quad的四个顶点
			int32 i0 = i * (k_MeshHNum * 2 + 1) + j;
			int32 i1 = i * (k_MeshHNum * 2 + 1) + (j + 1);
			int32 i2 = (i + 1) * (k_MeshHNum * 2 + 1) + (j + 1);
			int32 i3 = (i + 1) * (k_MeshHNum * 2 + 1) + j;

			// Triangle 1: v2 v1 v0
			indexArray[currIdx + 0] = i2;
			indexArray[currIdx + 1] = i1;
			indexArray[currIdx + 2] = i0;
			// Triangle 2: v3 v2 v0
			indexArray[currIdx + 3] = i3;
			indexArray[currIdx + 4] = i2;
			indexArray[currIdx + 5] = i0;

			currIdx += 6;
		}
	}

	FMeshDescription meshDesc;
	FStaticMeshAttributes attributes{meshDesc};
	attributes.Register();

	TVertexAttributesRef<FVector3f> posRef = attributes.GetVertexPositions();
	TMap<int32, FVertexID> vertexIDs;
	for (int i = 0; i < vertArray.Num(); i++)
	{
		FVertexID vertexID = meshDesc.CreateVertex();
		vertexIDs.Add(i, vertexID);
		posRef[vertexID] = vertArray[i];
	}

	FPolygonGroupID polyGroup = meshDesc.CreatePolygonGroup();
	// 设置polygon name, 之后设置材质要和这个name一样
	attributes.GetPolygonGroupMaterialSlotNames()[polyGroup] = FName("Default");

	TVertexInstanceAttributesRef<FVector2f> uvs0Ref = attributes.GetVertexInstanceUVs();
	uvs0Ref.SetNumChannels(1);

	const int32 k_PolygonTypeNum = 3;
	TArray<FVertexInstanceID> instanceIDs;
	instanceIDs.SetNum(k_PolygonTypeNum);

	currIdx = 0;
	for (int i = 0; i < indexArray.Num() / k_PolygonTypeNum; i++)
	{
		for (int j = 0; j < k_PolygonTypeNum; j++)
		{
			int32 idx = indexArray[currIdx];
			FVertexID vertexID = vertexIDs[idx];
			FVertexInstanceID vi = meshDesc.CreateVertexInstance(vertexID);
			instanceIDs[j] = vi;
			uvs0Ref.Set(vi, 0, uvArray[idx]);
			currIdx++;
		}

		meshDesc.CreatePolygon(polyGroup, instanceIDs);
	}

	// UE 重载了 '/', 这里用于路径拼接
	FString fullPackageName = packagePath / assetName;
	UPackage* package = CreatePackage(*fullPackageName);

	// 判断是否存在, 如果存在就做修改, 不存在就New
	UStaticMesh* staticMesh = FindObject<UStaticMesh>(package, *assetName);
	const bool bIsNewAsset = (staticMesh == nullptr);
	
	if (bIsNewAsset)
	{
		staticMesh = NewObject<UStaticMesh>(package, *assetName, RF_Public | RF_Standalone);
	}
	else
	{
		// 先释放资源, 注意要等待 然后释放渲染资源
		staticMesh->Modify(); // 记录修改
		staticMesh->ReleaseResources();
		FlushRenderingCommands(); // 等渲染线程彻底放掉旧资源
		staticMesh->GetStaticMaterials().Reset();
	}

	// staticMesh->bSupportRayTracing = 0;
	staticMesh->InitResources();
	staticMesh->SetNumSourceModels(1);

	FStaticMeshSourceModel& srcModel = staticMesh->GetSourceModel(0);
	srcModel.BuildSettings.bRecomputeNormals = true;
	srcModel.BuildSettings.bRecomputeTangents = true;
	srcModel.BuildSettings.bRemoveDegenerates = true;

	UMaterialInterface* defaultMaterial = UMaterial::GetDefaultMaterial(MD_Surface);
	staticMesh->GetStaticMaterials().Add(FStaticMaterial(defaultMaterial, FName("Default")));

	UStaticMesh::FBuildMeshDescriptionsParams params;
	params.bFastBuild = false;
	params.bUseHashAsGuid = true;
	params.bMarkPackageDirty = true;
	params.bCommitMeshDescription = true;
	params.bAllowCpuAccess = false;
	staticMesh->BuildFromMeshDescriptions({&meshDesc}, params);

	staticMesh->PostEditChange();
	staticMesh->MarkPackageDirty();

	FAssetRegistryModule::AssetCreated(staticMesh);
}

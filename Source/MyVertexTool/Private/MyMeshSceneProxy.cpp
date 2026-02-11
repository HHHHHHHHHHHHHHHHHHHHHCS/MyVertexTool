#include "MyMeshSceneProxy.h"
#include "Materials/MaterialRenderProxy.h"
#include "MyMeshConfig.h"
#include "MyMeshShaderCommon.h"
#include "MyMeshComponent.h"
#include "MyMeshBuffer.h"
#include "MyMeshVertexFactory.h"
#include "RenderCommandFence.h"

FMyMeshSceneProxy::FMyMeshSceneProxy(UMyMeshComponent* _myMeshComponent)
	: FPrimitiveSceneProxy(_myMeshComponent)
{
	UWorld* world = _myMeshComponent->GetWorld();
	if (!IsValid(world))
	{
		myMeshComponent = nullptr;
		myMeshVertexFactory = nullptr;
		return;
	}

	myMeshComponent = _myMeshComponent;
	myMeshVertexFactory = MakeUnique<FMyMeshVertexFactory>(world->GetFeatureLevel());
	// 开启WPO 顶点要做偏移
	bEvaluateWorldPositionOffset = myMeshComponent->bEvaluateWorldPositionOffset;
	BeginInitResource(myMeshVertexFactory.Get());
	InitMyMeshData();
}

FMyMeshSceneProxy::~FMyMeshSceneProxy()
{
	if (myMeshVertexFactory.IsValid())
	{
		myMeshVertexFactory->ReleaseResource();
		myMeshVertexFactory.Reset(nullptr);
	}
}

FPrimitiveViewRelevance FMyMeshSceneProxy::GetViewRelevance(const FSceneView* View) const
{
	FPrimitiveViewRelevance Result;
	Result.bDrawRelevance = IsShown(View);
	Result.bShadowRelevance = false; // 暂时不用阴影
	// bDynamicRelevance: 该 Primitive 需要在每帧的 GetDynamicMeshElements() 中生成 MeshBatch
	// 之后可以改成跟相机移动, 或者添加Culling, 所以这里暂时标记为动态
	Result.bDynamicRelevance = true;
	// bStaticRelevance: 该 Primitive 可以在场景初始化阶段生成 Static Mesh Draw Commands, 并长期复用
	// 通常在 DrawStaticElements() 中提交, 不随相机变化/不随参数变化/不随 InstanceBuffer 更新 位置和数量
	Result.bStaticRelevance = false;
	Result.bRenderInMainPass = ShouldRenderInMainPass();
	Result.bUsesLightingChannels = GetLightingChannelMask() != GetDefaultLightingChannelMask();
	Result.bRenderCustomDepth = ShouldRenderCustomDepth();
	Result.bTranslucentSelfShadow = bCastVolumetricTranslucentShadow;
	Result.bVelocityRelevance = DrawsVelocity() && Result.bOpaque && Result.bRenderInMainPass;
	return Result;
}

void FMyMeshSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, class FMeshElementCollector& Collector) const
{
	if (!myMeshVertexFactory.IsValid())
	{
		return;
	}

	QUICK_SCOPE_CYCLE_COUNTER(STAT_ProceduralMeshSceneProxy_GetDynamicMeshElements);

	// Set up wireframe material (if needed)
	const bool isEditor = ViewFamily.EngineShowFlags.Editor;
	const bool isWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;
	FMaterialRenderProxy* wireframeMaterialInstance = new FColoredMaterialRenderProxy(GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy() : NULL, FLinearColor(0, 0.5f, 1.f));
	Collector.RegisterOneFrameMaterialProxy(wireframeMaterialInstance);

	FRHICommandList& rhiCmdList = Collector.GetRHICommandList();

	UpdateParamsBuffer(rhiCmdList);

	// Gather visible tiles, their lod and materials for all renderable views (skip right view when stereo pair is rendered instanced)
	for (int32 viewIndex = 0; viewIndex < Views.Num(); viewIndex++)
	{
		if (VisibilityMap & (1 << viewIndex))
		{
			const FSceneView* view = Views[viewIndex];

			TObjectPtr<UMaterialInstance> myMaterial = myMeshComponent->GetMyMaterial();

			if (IsValid(myMaterial))
			{
				UpdateInstanceBuffer(rhiCmdList);

				FMaterialRenderProxy* materialProxy = nullptr;
				if (isWireframe)
				{
					materialProxy = wireframeMaterialInstance;
				}
				else
				{
					UMaterialInterface* material = myMaterial;
					materialProxy = material->GetRenderProxy();
				}

				bool bUseForDepthPass = false;
				// If there's a valid material, use that to figure out the depth pass status
				if (const FMaterial* BucketMaterial = materialProxy->GetMaterialNoFallback(
					GetScene().GetFeatureLevel()))
				{
					// Preemptively turn off depth rendering for this mesh batch if the material doesn't need it
					bUseForDepthPass = !BucketMaterial->GetShadingModels().HasShadingModel(MSM_SingleLayerWater) &&
						!IsTranslucentOnlyBlendMode(*BucketMaterial);
				}

				{
					FMeshBatch& Mesh = Collector.AllocateMesh();
					Mesh.bWireframe = isWireframe;
					Mesh.VertexFactory = myMeshVertexFactory.Get();
					Mesh.MaterialRenderProxy = materialProxy;
					Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
					Mesh.Type = PT_TriangleList;
					Mesh.DepthPriorityGroup = SDPG_World;
					Mesh.bCanApplyViewModeOverrides = true;
					Mesh.bUseForMaterial = true;
					Mesh.CastShadow = false; // 水暂时也不用阴影
					Mesh.bUseForDepthPass = bUseForDepthPass; // 其实本来水暂时不用写 PreDepth
					Mesh.bUseAsOccluder = false;

					// 默认值就有一个
					// Mesh.Elements.SetNumZeroed(1);
					FMeshBatchElement& BatchElement = Mesh.Elements[0];
					BatchElement.IndexBuffer = myMeshVertexFactory->GetIndexBuffer();
					BatchElement.NumInstances = myMeshVertexFactory->GetInstanceBuffer()->GetInstanceCount();
					BatchElement.PrimitiveUniformBuffer = GetUniformBuffer();
					BatchElement.FirstIndex = 0;
					BatchElement.NumPrimitives = myMeshVertexFactory->GetIndexBuffer()->GetIndexCount() / 3;
					BatchElement.MinVertexIndex = 0;
					BatchElement.MaxVertexIndex = myMeshVertexFactory->GetVertexBuffer()->GetVertexCount() - 1;

					Collector.AddMesh(viewIndex, Mesh);
				}
			}
		}
	}
}

void FMyMeshSceneProxy::InitMyMeshData() const
{
	// Params Buffer
	{
		FMyMeshVertexFactoryParameters paramsData;
		paramsData._LODMaxLevel = k_LODMaxLevel;
		paramsData._QuadWorldScale = k_QuadWorldScale;
		myMeshVertexFactory->GetParamsBuffer()->SetData(paramsData);
	}

	// Instance Data
	{
		FMyMeshInstanceBuffer* instanceBuffer = myMeshVertexFactory->GetInstanceBuffer();
		TArray<MyMeshInstanceType>& instanceDataArray = instanceBuffer->GetInstanceDataArray();

		float radiusStep = 50.0f;
		float radiusAcc = radiusStep;
		int32 dataIdx = 0;
		float scale = 1;

		instanceDataArray[dataIdx + 0] = MyMeshInstanceType{FVector2f{radiusAcc, radiusAcc}, scale};
		instanceDataArray[dataIdx + 1] = MyMeshInstanceType{FVector2f{-radiusAcc, radiusAcc}, scale};
		instanceDataArray[dataIdx + 2] = MyMeshInstanceType{FVector2f{radiusAcc, -radiusAcc}, scale};
		instanceDataArray[dataIdx + 3] = MyMeshInstanceType{FVector2f{-radiusAcc, -radiusAcc}, scale};

		radiusAcc += radiusStep;
		dataIdx += 4;
		for (int32 currLevel = 0; currLevel < k_LODMaxLevel; currLevel++)
		{
			radiusAcc += radiusStep;
			float quadStep = radiusStep * 2;

			instanceDataArray[dataIdx + 0] = MyMeshInstanceType{FVector2f{-radiusAcc, -radiusAcc}, scale};
			instanceDataArray[dataIdx + 1] = MyMeshInstanceType{FVector2f{-radiusAcc + quadStep, -radiusAcc}, scale};
			instanceDataArray[dataIdx + 2] = MyMeshInstanceType{FVector2f{+radiusAcc - quadStep, -radiusAcc}, scale};
			instanceDataArray[dataIdx + 3] = MyMeshInstanceType{FVector2f{+radiusAcc, -radiusAcc}, scale};

			instanceDataArray[dataIdx + 4] = MyMeshInstanceType{FVector2f{-radiusAcc, +radiusAcc}, scale};
			instanceDataArray[dataIdx + 5] = MyMeshInstanceType{FVector2f{-radiusAcc + quadStep, +radiusAcc}, scale};
			instanceDataArray[dataIdx + 6] = MyMeshInstanceType{FVector2f{+radiusAcc - quadStep, +radiusAcc}, scale};
			instanceDataArray[dataIdx + 7] = MyMeshInstanceType{FVector2f{+radiusAcc, +radiusAcc}, scale};

			instanceDataArray[dataIdx + 8] = MyMeshInstanceType{FVector2f{-radiusAcc, -radiusAcc + quadStep}, scale};
			instanceDataArray[dataIdx + 9] = MyMeshInstanceType{FVector2f{-radiusAcc, +radiusAcc - quadStep}, scale};

			instanceDataArray[dataIdx + 10] = MyMeshInstanceType{FVector2f{radiusAcc, -radiusAcc + quadStep}, scale};
			instanceDataArray[dataIdx + 11] = MyMeshInstanceType{FVector2f{radiusAcc, +radiusAcc - quadStep}, scale};

			radiusAcc += radiusStep;
			radiusStep *= 2;
			scale *= 2;
			dataIdx += 12;
		}

		instanceBuffer->SetDataDirty();
	}
}

void FMyMeshSceneProxy::UpdateInstanceBuffer(FRHICommandList& rhiCmdList) const
{
	myMeshVertexFactory->GetInstanceBuffer()->UpdateBuffer_RenderThread(rhiCmdList);
}

void FMyMeshSceneProxy::UpdateParamsBuffer(FRHICommandList& rhiCmdList) const
{
	myMeshVertexFactory->GetParamsBuffer()->UpdateBuffer_RenderThread(rhiCmdList);
}

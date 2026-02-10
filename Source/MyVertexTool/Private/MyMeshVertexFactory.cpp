#include "MyMeshVertexFactory.h"
#include "MeshDrawShaderBindings.h"
#include "MeshMaterialShader.h"
#include "MyMeshShaderCommon.h"
#include "MyMeshBuffer.h"

#pragma region VertexShader

IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FMyMeshVertexFactoryParameters, "MyMeshVF");

class FMyMeshVertexFactoryShaderParameters : public FVertexFactoryShaderParameters
{
	DECLARE_TYPE_LAYOUT(FMyMeshVertexFactoryShaderParameters, NonVirtual);

private:
	LAYOUT_FIELD(FShaderResourceParameter, _MyMeshInstanceBufferParam);

public:
	void Bind(const FShaderParameterMap& ParameterMap)
	{
		_MyMeshInstanceBufferParam.Bind(ParameterMap, TEXT("_MyMeshInstanceBuffer"));
	}

	void GetElementShaderBindings(
		const FSceneInterface* Scene,
		const FSceneView* View,
		const FMeshMaterialShader* Shader,
		const EVertexInputStreamType InputStreamType,
		ERHIFeatureLevel::Type FeatureLevel,
		const FVertexFactory* VertexFactory,
		const FMeshBatchElement& BatchElement,
		class FMeshDrawSingleShaderBindings& ShaderBindings,
		FVertexInputStreamArray& VertexStreams) const
	{
		FMyMeshVertexFactory* vertexFactory = (FMyMeshVertexFactory*)VertexFactory;
		if (vertexFactory)
		{
			ShaderBindings.Add(Shader->GetUniformBufferParameter<FMyMeshVertexFactoryParameters>(), vertexFactory->GetParamsBuffer()->GetUniformBuffer());
			ShaderBindings.Add(_MyMeshInstanceBufferParam, vertexFactory->GetInstanceBuffer()->GetInstanceBufferSRV());
			// 如果贴图要这样
			// ShaderBindings.AddTexture(VectorFieldTexture, VectorFieldTextureSampler, SamplerStatePoint, UserData->VectorFieldTextureRHI);
		}
	}
};

IMPLEMENT_TYPE_LAYOUT(FMyMeshVertexFactoryShaderParameters);

IMPLEMENT_VERTEX_FACTORY_PARAMETER_TYPE(FMyMeshVertexFactory, SF_Vertex, FMyMeshVertexFactoryShaderParameters);

IMPLEMENT_VERTEX_FACTORY_TYPE(FMyMeshVertexFactory, "/Plugin/MyVertexTool/Private/MyMeshVertexFactory.ush",
							EVertexFactoryFlags::UsedWithMaterials
							| EVertexFactoryFlags::SupportsStaticLighting
							| EVertexFactoryFlags::SupportsDynamicLighting
							| EVertexFactoryFlags::SupportsPrecisePrevWorldPos
							| EVertexFactoryFlags::SupportsPositionOnly
							| EVertexFactoryFlags::SupportsCachingMeshDrawCommands
							| EVertexFactoryFlags::SupportsPrimitiveIdStream
							// 如果要支持 Ray Tracing, 理论上需要 Ray Tracing Intersection Shader
							// 但是因为存在 WPO, 会导致需要持续更新加速结构, 成本较高
							// 所以暂时不支持
							// | EVertexFactoryFlags::SupportsRayTracing
							// | EVertexFactoryFlags::SupportsRayTracingDynamicGeometry
							| EVertexFactoryFlags::SupportsLightmapBaking
							| EVertexFactoryFlags::SupportsManualVertexFetch
							| EVertexFactoryFlags::SupportsPSOPrecaching
							| EVertexFactoryFlags::SupportsGPUSkinPassThrough
							| EVertexFactoryFlags::SupportsLumenMeshCards
							| EVertexFactoryFlags::SupportsTriangleSorting);

#pragma endregion VertexShader


FMyMeshVertexFactory::FMyMeshVertexFactory(ERHIFeatureLevel::Type InFeatureLevel)
	: FVertexFactory(InFeatureLevel)
{
	vertexBuffer = MakeUnique<FMyMeshVertexBuffer>();
	indexBuffer = MakeUnique<FMyMeshIndexBuffer>();
	instanceBuffer = MakeUnique<FMyMeshInstanceBuffer>();
	paramsBuffer = MakeUnique<FMyMeshParamsBuffer>();
}

FMyMeshVertexFactory::~FMyMeshVertexFactory()
{
	vertexBuffer.Reset(nullptr);
	indexBuffer.Reset(nullptr);
	instanceBuffer.Reset(nullptr);
	paramsBuffer.Reset(nullptr);
}

bool FMyMeshVertexFactory::ShouldCompilePermutation(const FVertexFactoryShaderPermutationParameters& Parameters)
{
	return true;

	/*
	if (!IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::ES3_1))
	{
		return false;
	}

	if (!Parameters.MaterialParameters.bIsUsedMyMesh)
	{
		return false;
	}

	const FName shaderTypeName = Parameters.ShaderType->GetName();

	static const TArray<FName> k_DisableShaderType =
	{
		FName(TEXT("TShadowDepthVSVertexShadowDepth_VirtualShadowMap")),
		FName(TEXT("TShadowDepthVSVertexShadowDepth_VirtualShadowMapPositionOnly")),
		FName(TEXT("TShadowDepthPSPixelShadowDepth_VirtualShadowMap")),
		FName(TEXT("TShadowDepthVSVertexShadowDepth_OnePassPointLight")),
		FName(TEXT("TShadowDepthVSVertexShadowDepth_OnePassPointLightPositionOnly")),
		FName(TEXT("TShadowDepthPSPixelShadowDepth_OnePassPointLight")),
		FName(TEXT("TShadowDepthVS")),
		FName(TEXT("TShadowDepthVSVertexShadowDepth_OutputDepth")),
		FName(TEXT("TShadowDepthVSVertexShadowDepth_OutputDepthPositionOnly")),
		FName(TEXT("TShadowDepthVSVertexShadowDepth_PerspectiveCorrect")),
		FName(TEXT("TShadowDepthVSVertexShadowDepth_PerspectiveCorrectPositionOnly")),
		FName(TEXT("TShadowDepthPSPixelShadowDepth_PerspectiveCorrect")),
		FName(TEXT("TShadowDepthPSPixelShadowDepth_NonPerspectiveCorrect")),
	};

	for (const FName& item : k_DisableShaderType)
	{
		if (shaderTypeName == item)
		{
			return false;
		}
	}
	return true;
	*/
}

void FMyMeshVertexFactory::ModifyCompilationEnvironment(const FVertexFactoryShaderPermutationParameters& Parameters,
														FShaderCompilerEnvironment& OutEnvironment)
{
	// if (IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::ES3_1))
	// {
	// 	OutEnvironment.SetDefine(TEXT("USE_LOW_QUALITY"), 1);
	// }

	// const bool bVFSupportsPrimitiveSceneData = Parameters.VertexFactoryType->SupportsPrimitiveIdStream() && UseGPUScene(Parameters.Platform, GetMaxSupportedFeatureLevel(Parameters.Platform));
	// OutEnvironment.SetDefine(TEXT("VF_SUPPORTS_PRIMITIVE_SCENE_DATA"), bVFSupportsPrimitiveSceneData);

	// When combining ray tracing and WPO, leave the mesh in local space for consistency with how shading normals are calculated.
	// See UE-139634 for the case that lead to this.
	// OutEnvironment.SetDefine(TEXT("RAY_TRACING_DYNAMIC_MESH_IN_LOCAL_SPACE"), TEXT("1"));
}

void FMyMeshVertexFactory::ValidateCompiledResult(const FVertexFactoryType* Type, EShaderPlatform Platform,
												const FShaderParameterMap& ParameterMap, TArray<FString>& OutErrors)
{
	/*
	if (ParameterMap.ContainsParameterAllocation(FSceneTextureUniformParameters::FTypeInfo::GetStructMetadata()->GetShaderVariableName()))
	{
		OutErrors.Add(TEXT("Ray tracing callable shaders cannot read from the SceneTexturesStruct."));
		return ;
	}

	for (const auto& It : ParameterMap.GetParameterMap())
	{
		const FParameterAllocation& ParamAllocation = It.Value;
		if (ParamAllocation.Type != EShaderParameterType::UniformBuffer
			&& ParamAllocation.Type != EShaderParameterType::LooseData)
		{
			OutErrors.Add(FString::Printf(TEXT("Invalid ray tracing shader parameter '%s'. Only uniform buffers and loose data parameters are supported."), *(It.Key)));
			return;
		}
	}
	*/
}

void FMyMeshVertexFactory::GetPSOPrecacheVertexFetchElements(EVertexInputStreamType VertexInputStreamType,
															FVertexDeclarationElementList& Elements)
{
	// 确保在PSO预缓存阶段收集的顶点声明与运行时实际渲染使用的顶点声明完全一致
	// https://dev.epicgames.com/documentation/zh-cn/unreal-engine/pso-precaching-for-unreal-engine
	// VertexInputStreamType == Default / PositionOnly / PositionAndNormalOnly,
	Elements.Add(FVertexElement(0, 0, VET_Float3, 0, sizeof(float) * 3u, false));

	if (UseGPUScene(GMaxRHIShaderPlatform, GMaxRHIFeatureLevel)
		&& !PlatformGPUSceneUsesUniformBufferView(GMaxRHIShaderPlatform))
	{
		switch (VertexInputStreamType)
		{
		case EVertexInputStreamType::Default:
			{
				Elements.Add(FVertexElement(1, 0, VET_UInt, 13, sizeof(uint32), true));
				break;
			}
		case EVertexInputStreamType::PositionOnly:
		case EVertexInputStreamType::PositionAndNormalOnly:
			{
				Elements.Add(FVertexElement(1, 0, VET_UInt, 1, sizeof(uint32), true));
				break;
			}
		default:
			checkNoEntry();
		}
	}
}

void FMyMeshVertexFactory::InitRHI(FRHICommandListBase& RHICmdList)
{
	FVertexFactory::InitRHI(RHICmdList);

	vertexBuffer->InitResource(RHICmdList);
	indexBuffer->InitResource(RHICmdList);
	instanceBuffer->InitResource(RHICmdList);
	paramsBuffer->InitResource(RHICmdList);

	// No streams should currently exist.
	check(Streams.Num() == 0);

	FMyMeshA2VType a2vType;
	a2vType.positionComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(vertexBuffer.Get(), MyMeshVertexType, position, VET_Float3);

	FVertexDeclarationElementList Elements;
	Elements.Add(AccessStreamComponent(a2vType.positionComponent, 0));

	InitDeclaration(Elements);
}

void FMyMeshVertexFactory::ReleaseRHI()
{
	if (vertexBuffer.IsValid())
	{
		vertexBuffer->ReleaseResource();
	}

	if (indexBuffer.IsValid())
	{
		indexBuffer->ReleaseResource();
	}

	if (instanceBuffer.IsValid())
	{
		instanceBuffer->ReleaseResource();
	}

	if (paramsBuffer.IsValid())
	{
		paramsBuffer->ReleaseResource();
	}

	FVertexFactory::ReleaseRHI();
}

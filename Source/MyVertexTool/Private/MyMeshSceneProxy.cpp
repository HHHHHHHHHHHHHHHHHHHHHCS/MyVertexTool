#include "MyMeshVertexFactory.h"
#include "MyMeshSceneProxy.h"
#include "MyMeshComponent.h"
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

FMyMeshSceneProxy::FMyMeshSceneProxy(UMyMeshComponent* _myMeshComponent)
	: FPrimitiveSceneProxy(_myMeshComponent)
{
}

FMyMeshSceneProxy::~FMyMeshSceneProxy()
{
}

FPrimitiveViewRelevance FMyMeshSceneProxy::GetViewRelevance(const FSceneView* View) const
{
	// TODO:
	return FPrimitiveSceneProxy::GetViewRelevance(View);
}

void FMyMeshSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, class FMeshElementCollector& Collector) const
{
	// TODO:
	FPrimitiveSceneProxy::GetDynamicMeshElements(Views, ViewFamily, VisibilityMap, Collector);
}

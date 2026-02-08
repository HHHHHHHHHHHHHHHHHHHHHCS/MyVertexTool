#pragma once
#include "MyMeshConfig.h"
#include "MyMeshShaderCommon.h"

class FMyMeshVertexBuffer final : public FVertexBuffer
{
public:
	virtual void InitRHI(FRHICommandListBase& RHICmdList) override;

	UE_NODISCARD_CTOR FORCEINLINE uint32 GetVertexCount() const { return k_VertexCount; }
};

class FMyMeshIndexBuffer final : public FIndexBuffer
{
public:
	virtual void InitRHI(FRHICommandListBase& RHICmdList) override;

	template <typename IndexType>
	FBufferRHIRef CreateIndexBuffer(FRHICommandListBase& RHICmdList);

	UE_NODISCARD_CTOR FORCEINLINE uint32 GetIndexCount() const { return k_IndexCount; }
};

class FMyMeshInstanceBuffer final : public FRenderResource
{
public:
	FMyMeshInstanceBuffer();

	UE_NODISCARD_CTOR FORCEINLINE uint32 GetInstanceCount() const { return k_QuadCount; }
};


class FMyMeshParamsBuffer final : public FRenderResource
{
private:
	bool isDataDirty = false;
	FMyMeshVertexFactoryParameters myMeshVertexFactoryParameters;
	FUniformBufferRHIRef uniformBufferRHI;

public:
	FMyMeshParamsBuffer();

	virtual void InitRHI(FRHICommandListBase& RHICmdList) override;

	virtual void ReleaseRHI() override;

	void SetData(const FMyMeshVertexFactoryParameters& newParams);

	void UpdateBuffer(FRHICommandList& rhiCmdList);

	UE_NODISCARD_CTOR FORCEINLINE FUniformBufferRHIRef GetUniformBuffer() const { return uniformBufferRHI; }
};

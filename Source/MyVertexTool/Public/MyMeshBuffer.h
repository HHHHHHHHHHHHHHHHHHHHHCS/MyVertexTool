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
private:
	bool isDataDirty;
	TArray<MyMeshInstanceType> instanceDataArray;
	FBufferRHIRef instanceBufferRHI;
	FShaderResourceViewRHIRef instanceBufferSRV;
	
public:
	FMyMeshInstanceBuffer();

	void OnInit();

	virtual void InitRHI(FRHICommandListBase& RHICmdList) override;
	virtual void ReleaseRHI() override;

	void UpdateBuffer_RenderThread(FRHICommandList& RHICmdList);

	FORCEINLINE void SetDataDirty(bool _isDataDirty = true) { isDataDirty = _isDataDirty; }

	UE_NODISCARD_CTOR FORCEINLINE uint32 GetInstanceCount() const { return k_QuadCount; }
	UE_NODISCARD_CTOR FORCEINLINE uint32 GetInstanceBufferSize() const { return GetInstanceCount() * sizeof(MyMeshInstanceType); }
	UE_NODISCARD_CTOR FORCEINLINE TArray<MyMeshInstanceType>& GetInstanceDataArray() { return instanceDataArray; }
	UE_NODISCARD_CTOR FORCEINLINE FRHIShaderResourceView* GetInstanceBufferSRV() const { return instanceBufferSRV; }
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

	void UpdateBuffer_RenderThread(FRHICommandList& rhiCmdList);

	UE_NODISCARD_CTOR FORCEINLINE FUniformBufferRHIRef GetUniformBuffer() const { return uniformBufferRHI; }
};

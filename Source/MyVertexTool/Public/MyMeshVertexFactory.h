#pragma once

class FMyMeshVertexBuffer;
class FMyMeshIndexBuffer;
class FMyMeshInstanceBuffer;
class FMyMeshParamsBuffer;

class FMyMeshVertexFactory final : public FVertexFactory
{
	DECLARE_VERTEX_FACTORY_TYPE(FMyMeshVertexFactory);

public:
	using Super = FVertexFactory;

private:
	TUniquePtr<FMyMeshVertexBuffer> vertexBuffer;
	TUniquePtr<FMyMeshIndexBuffer> indexBuffer;
	TUniquePtr<FMyMeshInstanceBuffer> instanceBuffer;
	TUniquePtr<FMyMeshParamsBuffer> paramsBuffer;

public:
	FMyMeshVertexFactory(ERHIFeatureLevel::Type InFeatureLevel);

	virtual ~FMyMeshVertexFactory() override;

	static bool ShouldCompilePermutation(const FVertexFactoryShaderPermutationParameters& Parameters);

	static void ModifyCompilationEnvironment(const FVertexFactoryShaderPermutationParameters& Parameters,
											FShaderCompilerEnvironment& OutEnvironment);

	static void ValidateCompiledResult(const FVertexFactoryType* Type, EShaderPlatform Platform,
										const FShaderParameterMap& ParameterMap, TArray<FString>& OutErrors);

	static void GetPSOPrecacheVertexFetchElements(EVertexInputStreamType VertexInputStreamType,
												FVertexDeclarationElementList& Elements);

	virtual void InitRHI(FRHICommandListBase& RHICmdList) override;
	virtual void ReleaseRHI() override;

	UE_NODISCARD_CTOR FORCEINLINE FMyMeshVertexBuffer* GetVertexBuffer() const { return vertexBuffer.Get(); }
	UE_NODISCARD_CTOR FORCEINLINE FMyMeshIndexBuffer* GetIndexBuffer() const { return indexBuffer.Get(); }
	UE_NODISCARD_CTOR FORCEINLINE FMyMeshInstanceBuffer* GetInstanceBuffer() const { return instanceBuffer.Get(); }
	UE_NODISCARD_CTOR FORCEINLINE FMyMeshParamsBuffer* GetUniformBuffer() const { return paramsBuffer.Get(); }
};

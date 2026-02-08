#include "MyMeshVertexFactory.h"

FMyMeshVertexFactory::FMyMeshVertexFactory(ERHIFeatureLevel::Type InFeatureLevel)
	: FVertexFactory(InFeatureLevel)
{
}

FMyMeshVertexFactory::~FMyMeshVertexFactory()
{
}

bool FMyMeshVertexFactory::ShouldCompilePermutation(const FVertexFactoryShaderPermutationParameters& Parameters)
{
	return true;
}

void FMyMeshVertexFactory::ModifyCompilationEnvironment(const FVertexFactoryShaderPermutationParameters& Parameters,
														FShaderCompilerEnvironment& OutEnvironment)
{
}

void FMyMeshVertexFactory::ValidateCompiledResult(const FVertexFactoryType* Type, EShaderPlatform Platform,
												const FShaderParameterMap& ParameterMap, TArray<FString>& OutErrors)
{
}

void FMyMeshVertexFactory::GetPSOPrecacheVertexFetchElements(EVertexInputStreamType VertexInputStreamType,
															FVertexDeclarationElementList& Elements)
{
}

void FMyMeshVertexFactory::InitRHI(FRHICommandListBase& RHICmdList)
{
	FVertexFactory::InitRHI(RHICmdList);
}

void FMyMeshVertexFactory::ReleaseRHI()
{
	FVertexFactory::ReleaseRHI();
}

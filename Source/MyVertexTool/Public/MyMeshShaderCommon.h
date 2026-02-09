#pragma once

struct MyMeshVertexType
{
public:
	FVector3f position;
};

// UE 顶点阶段 不怎么支持 StructuredBuffer, 所以这里用 Buffer<float4> Pacv/Unpack
// Buffer<float4> 需要 float4 对齐
// 注意要写空的构造函数, TArray 需要
struct MyMeshInstanceType
{
public:
	FVector2f posWSOffset;
	float scale;
	float padding0;

	MyMeshInstanceType(FVector2f _posWSOffset, float _scale)
		: posWSOffset(_posWSOffset)
		, scale(_scale)
		, padding0(0)
	{
	}

	MyMeshInstanceType()
		: posWSOffset(FVector2f{})
		, scale(0)
		, padding0(0)
	{
	}
};

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FMyMeshVertexFactoryParameters,)
	SHADER_PARAMETER(int32, _LODMaxLevel)
	SHADER_PARAMETER(int32, _QuadWorldScale)
END_GLOBAL_SHADER_PARAMETER_STRUCT()

typedef TUniformBufferRef<FMyMeshVertexFactoryParameters> FMyMeshVertexFactoryBufferRef;

struct FMyMeshA2VType
{
	FVertexStreamComponent positionComponent;
};

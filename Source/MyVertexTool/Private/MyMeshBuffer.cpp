#include "MyMeshBuffer.h"

void FMyMeshVertexBuffer::InitRHI(FRHICommandListBase& RHICmdList)
{
	// 创建[-0.5,-0.5]~[0.5,0.5]的单位vertex, ue 100cm = 1m
	constexpr float k_VertexStartPosX = -50.0f;
	constexpr float k_VertexStartPosY = -50.0f;
	constexpr float k_VertexStep = 100.0f / (k_PointCount - 1);

	const FRHIBufferCreateDesc createDesc =
		FRHIBufferCreateDesc::CreateVertex<MyMeshVertexType>(TEXT("MyMeshVertexBuffer"), GetVertexCount())
		.AddUsage(EBufferUsageFlags::Static | EBufferUsageFlags::ShaderResource)
		.SetInitialState(ERHIAccess::VertexOrIndexBuffer | ERHIAccess::SRVMask)
		.SetInitActionInitializer();

	TRHIBufferInitializer<MyMeshVertexType> initializer = RHICmdList.CreateBufferInitializer(createDesc);

	int32 vertexIdx = 0;
	for (int32 y = 0; y < k_PointCount; y++)
	{
		float vertexY = k_VertexStartPosY + y * k_VertexStep;
		for (int32 x = 0; x < k_PointCount; x++)
		{
			initializer[vertexIdx] = MyMeshVertexType
			{
				.position = FVector3f{k_VertexStartPosX + x * k_VertexStep, vertexY, 0},
			};
			vertexIdx++;
		}
	}

	VertexBufferRHI = initializer.Finalize();
}

void FMyMeshIndexBuffer::InitRHI(FRHICommandListBase& RHICmdList)
{
	if (GetIndexCount() <= UINT16_MAX)
	{
		IndexBufferRHI = CreateIndexBuffer<uint16>(RHICmdList);
	}
	else
	{
		IndexBufferRHI = CreateIndexBuffer<uint32>(RHICmdList);
	}
}

template <typename IndexType>
FBufferRHIRef FMyMeshIndexBuffer::CreateIndexBuffer(FRHICommandListBase& RHICmdList)
{
	const FRHIBufferCreateDesc createDesc =
		FRHIBufferCreateDesc::CreateIndex<IndexType>(TEXT("MyMeshIndexBuffer"), GetIndexCount())
		.AddUsage(EBufferUsageFlags::Static)
		.SetInitialState(ERHIAccess::VertexOrIndexBuffer)
		.SetInitActionInitializer();

	TRHIBufferInitializer<IndexType> initializer = RHICmdList.CreateBufferInitializer(createDesc);

	int32 indexIdx = 0;
	for (int32 y = 0; y < k_TessQuadCount; y++)
	{
		for (int32 x = 0; x < k_TessQuadCount; x++)
		{
			int32 vertexIdx = y * k_PointCount + x;
			initializer[indexIdx + 0] = vertexIdx + 0;
			initializer[indexIdx + 1] = vertexIdx + k_PointCount + 1;
			initializer[indexIdx + 2] = vertexIdx + 1;
			initializer[indexIdx + 3] = vertexIdx + 0;
			initializer[indexIdx + 4] = vertexIdx + k_PointCount;
			initializer[indexIdx + 5] = vertexIdx + k_PointCount + 1;
			indexIdx += 6;
		}
	}

	return initializer.Finalize();
}

FMyMeshInstanceBuffer::FMyMeshInstanceBuffer()
{
	OnInit();
}

void FMyMeshInstanceBuffer::OnInit()
{
	instanceDataArray.SetNum(GetInstanceCount());
}

void FMyMeshInstanceBuffer::InitRHI(FRHICommandListBase& RHICmdList)
{
	constexpr uint32 k_Stride = 4 * sizeof(float);
	const uint32 k_Num = GetInstanceBufferSize() / k_Stride;

	// 为什么这么写
	// 1. 因为 UE 顶点流程不支持变体, 用模板替代
	// 2. 顶点阶段不怎么支持StructuredBuffer, 用 buffer<float4> Pack/Unpack 替代, 同时要注意 float4 对齐
	//       在Mobile Vulkan上bound有问题 会被当做RWBuffer, 以至于StructuredBuffer绑不上去
	FRHIBufferDesc bufferDesc{GetInstanceBufferSize(), k_Stride, EBufferUsageFlags::None};
	const FRHIBufferCreateDesc createDesc =
		FRHIBufferCreateDesc::Create(TEXT("MyMeshInstanceBuffer"), bufferDesc)
		.AddUsage(EBufferUsageFlags::Dynamic | EBufferUsageFlags::ShaderResource)
		.SetInitialState(ERHIAccess::SRVGraphics);
	// 第一次的时候会刷入数据没有必要初始化, 而且 如果是 ZeroData Vulkan 会延迟初始化 导致空指针报错
	// .SetInitActionZeroData();

	instanceBufferRHI = RHICmdList.CreateBuffer(createDesc);

	FRHIViewDesc::FBufferSRV::FInitializer viewDesc =
		FRHIViewDesc::CreateBufferSRV()
		.SetType(FRHIViewDesc::EBufferType::Typed)
		.SetFormat(EPixelFormat::PF_A32B32G32R32F)
		.SetNumElements(k_Num);
	instanceBufferSRV = RHICmdList.CreateShaderResourceView(instanceBufferRHI, viewDesc);
}

void FMyMeshInstanceBuffer::ReleaseRHI()
{
	if (instanceBufferRHI.IsValid())
	{
		instanceBufferRHI.SafeRelease();
	}
	if (instanceBufferSRV.IsValid())
	{
		instanceBufferSRV.SafeRelease();
	}
}

void FMyMeshInstanceBuffer::UpdateBuffer_RenderThread(FRHICommandList& RHICmdList)
{
	if (!isDataDirty)
	{
		return;
	}
	isDataDirty = false;
	void* destBuffer = RHICmdList.LockBuffer(instanceBufferRHI, 0, GetInstanceBufferSize(), RLM_WriteOnly);
	FMemory::Memcpy(destBuffer, instanceDataArray.GetData(), GetInstanceBufferSize());
	RHICmdList.UnlockBuffer(instanceBufferRHI);
}

FMyMeshParamsBuffer::FMyMeshParamsBuffer()
{
}

void FMyMeshParamsBuffer::InitRHI(FRHICommandListBase& RHICmdList)
{
	// RHICreateUniformBuffer 有引用计数器, 会自动 MarkForDelete 并 Release
	uniformBufferRHI = RHICreateUniformBuffer(
		&myMeshVertexFactoryParameters, &FMyMeshVertexFactoryParameters::GetStructMetadata()->GetLayout(), UniformBuffer_MultiFrame);
}

void FMyMeshParamsBuffer::ReleaseRHI()
{
	uniformBufferRHI.SafeRelease();
}

void FMyMeshParamsBuffer::SetData(const FMyMeshVertexFactoryParameters& newParams)
{
	myMeshVertexFactoryParameters = newParams;
	isDataDirty = true;
}

void FMyMeshParamsBuffer::UpdateBuffer_RenderThread(FRHICommandList& rhiCmdList)
{
	if (!isDataDirty)
	{
		return;
	}
	isDataDirty = false;
	rhiCmdList.UpdateUniformBuffer(uniformBufferRHI, &myMeshVertexFactoryParameters);
}

#pragma once

class UMyMeshComponent;
class FMyMeshVertexFactory;

class FMyMeshSceneProxy final : public FPrimitiveSceneProxy
{
private:
	TObjectPtr<UMyMeshComponent> myMeshComponent;
	TUniquePtr<FMyMeshVertexFactory> myMeshVertexFactory;

public:
	FMyMeshSceneProxy(UMyMeshComponent* _myMeshComponent);

	virtual ~FMyMeshSceneProxy() override;

	virtual SIZE_T GetTypeHash() const override
	{
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	}

	virtual uint32 GetMemoryFootprint() const override
	{
		return sizeof(*this) + NewGetAllocatedSize();
	}

	uint32 NewGetAllocatedSize() const
	{
		return (uint32)(FPrimitiveSceneProxy::GetAllocatedSize());
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;

	virtual void GetDynamicMeshElements(
		const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap,
		class FMeshElementCollector& Collector) const override;
};

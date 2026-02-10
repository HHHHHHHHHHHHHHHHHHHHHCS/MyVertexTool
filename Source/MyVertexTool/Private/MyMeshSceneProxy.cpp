#include "MyMeshSceneProxy.h"
#include "MyMeshComponent.h"

FMyMeshSceneProxy::FMyMeshSceneProxy(UMyMeshComponent* _myMeshComponent)
	: FPrimitiveSceneProxy(_myMeshComponent)
{
	// TODO:
}

FMyMeshSceneProxy::~FMyMeshSceneProxy()
{
	// TODO:
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

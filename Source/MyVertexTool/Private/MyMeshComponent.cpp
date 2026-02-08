#include "MyMeshComponent.h"
#include "MyMeshConfig.h"
#include "MyMeshSceneProxy.h"

UMyMeshComponent::UMyMeshComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

UMyMeshComponent::~UMyMeshComponent()
{
}

void UMyMeshComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMyMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FPrimitiveSceneProxy* UMyMeshComponent::CreateSceneProxy()
{
	return new FMyMeshSceneProxy(this);
}

FBoxSphereBounds UMyMeshComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	// 因为这个是extent(半尺寸) 所以要*2
	const float k_AABBExtent = 100 * 2 * k_MaxWorldScale;

	FBoxSphereBounds newBounds;
	newBounds.Origin = FVector{0, 0, k_WorldHeightCenter};
	newBounds.BoxExtent = FVector{k_AABBExtent, k_AABBExtent, k_WorldHeightExtent};
	newBounds.SphereRadius = 1.414214f * k_AABBExtent; // 1.414 = sqrt(2)
	return newBounds.TransformBy(LocalToWorld);
}

void UMyMeshComponent::GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials, bool bGetDebugMaterials) const
{
	OutMaterials.Add(GetMyMaterial());
}

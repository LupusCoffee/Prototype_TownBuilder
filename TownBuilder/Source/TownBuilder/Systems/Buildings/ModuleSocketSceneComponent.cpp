#include "Systems/Buildings/ModuleSocketSceneComponent.h"
#include "GameFramework/Actor.h"


//Setup
UModuleSocketSceneComponent::UModuleSocketSceneComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


//Overrides
void UModuleSocketSceneComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	ClearConnectedSocket();

	Super::OnComponentDestroyed(bDestroyingHierarchy);
}


//Socket
bool UModuleSocketSceneComponent::Accepts(FGameplayTag ModuleType) const
{
	if (!ModuleType.IsValid()) return false;

	return ModuleType.MatchesAny(AllowedModuleTypes);
}

void UModuleSocketSceneComponent::SetConnectedSocket(UModuleSocketSceneComponent* OtherSocket)
{
	if (!OtherSocket) return;
	if (OtherSocket == this) return;

	ClearConnectedSocket();
	OtherSocket->ClearConnectedSocket();

	ConnectedSocket = OtherSocket;
	OtherSocket->ConnectedSocket = this;
}

void UModuleSocketSceneComponent::ClearConnectedSocket()
{
	if (IsFree()) return;

	UModuleSocketSceneComponent* OtherSocket = ConnectedSocket;
	ConnectedSocket = nullptr;

	if (OtherSocket->ConnectedSocket == this) OtherSocket->ConnectedSocket = nullptr;
}


//Getters
FVector UModuleSocketSceneComponent::GetOutwardDirection() const
{
	const AActor* Owner = GetOwner();
	if (!Owner) return GetForwardVector();
	if (!GetRelativeRotation().IsNearlyZero()) return GetForwardVector();

	const FTransform& OwnerTransform = Owner->GetActorTransform();
	const FBox LocalBounds = Owner->CalculateComponentsBoundingBoxInLocalSpace(true);
	const FVector LocalExtent = LocalBounds.GetExtent().ComponentMax(FVector(KINDA_SMALL_NUMBER));
	const FVector LocalOffset = OwnerTransform.InverseTransformPosition(GetComponentLocation()) - LocalBounds.GetCenter();
	const FVector AbsoluteOffset = LocalOffset.GetAbs() / LocalExtent;

	FVector LocalDirection = FVector(LocalOffset.X, 0.f, 0.f);

	if (AbsoluteOffset.Y > AbsoluteOffset.X && AbsoluteOffset.Y >= AbsoluteOffset.Z)
	{
		LocalDirection = FVector(0.f, LocalOffset.Y, 0.f);
	}
	else if (AbsoluteOffset.Z > AbsoluteOffset.X && AbsoluteOffset.Z > AbsoluteOffset.Y)
	{
		LocalDirection = FVector(0.f, 0.f, LocalOffset.Z);
	}

	return OwnerTransform.TransformVectorNoScale(LocalDirection.GetSafeNormal());
}

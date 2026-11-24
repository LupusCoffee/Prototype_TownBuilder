#include "Systems/Buildings/BuildComponent.h"
#include "Components/MeshComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Systems/Buildings/Building.h"
#include "Systems/Buildings/BuildingCatalogSubsystem.h"
#include "Systems/Buildings/ModuleSocketSceneComponent.h"
#include "Systems/Core/TraceLibrary.h"


//Setup
UBuildComponent::UBuildComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UBuildComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<APlayerController>(GetOwner());
}

void UBuildComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ReleaseHeld();

	Super::EndPlay(EndPlayReason);
}


//Tick
void UBuildComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (HeldBuilding) UpdateHeldLocation();
}


//Overrides
void UBuildComponent::ExitTool_Implementation()
{
	CancelHeld();
}


//Placement
void UBuildComponent::StartBuilding(FName BuildingTypeId)
{
	ReleaseHeld();

	UBuildingCatalogSubsystem* Catalog = GetCatalog();
	if (!Catalog) return;

	const FBuildingEntry Entry = Catalog->GetBuilding(BuildingTypeId);
	if (Entry.BuildingTypeId.IsNone() || !Entry.Data.ActorClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("BuildComponent: No building or actor class for '%s'."), *BuildingTypeId.ToString());
		return;
	}

	ABuilding* Building = SpawnBuilding(Entry.Data.ActorClass, FTransform(GetHeldRotation()), BuildingTypeId);
	if (!Building) return;

	Hold(Building, false);
	OnHoldStarted.Broadcast(BuildingTypeId);
}

void UBuildComponent::StartMovingBuildingUnderCursor()
{
	ABuilding* ClickedBuilding = UTraceLibrary::GetActorUnderCursor<ABuilding>(PlayerController);
	if (!ClickedBuilding) return;

	ABuilding* Building = ClickedBuilding->GetRootBuilding();

	ReleaseHeld();

	HeldOriginalTransform = Building->GetActorTransform();
	HeldSpin = HeldOriginalTransform.Rotator().Yaw;

	Hold(Building, true);
	OnHoldStarted.Broadcast(Building->GetBuildingTypeId());
}

void UBuildComponent::CancelHeld()
{
	if (!HeldBuilding) return;

	ReleaseHeld();
	OnHoldEnded.Broadcast();
}

ABuilding* UBuildComponent::PlaceHeld()
{
	if (!HeldBuilding || !bCanPlaceHeld) return nullptr;

	ABuilding* Building = HeldBuilding;
	const bool bWasExisting = bHeldIsExisting;

	ConnectBuilding(Building, CurrentTargetSocket, CurrentHeldBuildingSocket);
	Drop();

	if (bWasExisting)
	{
		OnBuildingMoved.Broadcast(Building);
		OnHoldEnded.Broadcast();
		return Building;
	}

	OnBuildingPlaced.Broadcast(Building);

	if (!bKeepBuildingAfterPlace)
	{
		OnHoldEnded.Broadcast();
		return Building;
	}

	ABuilding* NextBuilding = SpawnBuilding(Building->GetClass(), Building->GetActorTransform(), Building->GetBuildingTypeId());
	if (NextBuilding) Hold(NextBuilding, false);
	else OnHoldEnded.Broadcast();

	return Building;
}

void UBuildComponent::RotateHeld(float Direction)
{
	HeldSpin = FRotator::NormalizeAxis(HeldSpin + Direction * RotationStep);

	if (HeldBuilding) HeldBuilding->SetActorRotation(GetHeldRotation());
}


//Helpers
void UBuildComponent::Hold(ABuilding* Building, bool bIsExisting)
{
	HeldBuilding = Building;
	bHeldIsExisting = bIsExisting;
	bCanPlaceHeld = false;

	for (ABuilding* TreeBuilding : HeldBuilding->GetBuildingTree())
	{
		HeldTree.Add(TreeBuilding);

		TArray<UMeshComponent*> MeshComponents;
		TreeBuilding->GetComponents<UMeshComponent>(MeshComponents);

		for (UMeshComponent* MeshComponent : MeshComponents)
		{
			FHeldMeshMaterials& Saved = HeldMaterials.AddDefaulted_GetRef();
			Saved.Mesh = MeshComponent;

			for (int32 MaterialIndex = 0; MaterialIndex < MeshComponent->GetNumMaterials(); MaterialIndex++)
			{
				Saved.Materials.Add(MeshComponent->GetMaterial(MaterialIndex));
			}
		}

		TreeBuilding->SetActorEnableCollision(false);
	}

	CacheHeldPlacementRules();
	ApplyHeldMaterial();
	UpdateHeldLocation();
}

void UBuildComponent::CacheHeldPlacementRules()
{
	HeldModuleType = FGameplayTag();
	bHeldGroundPlaceable = true;

	UBuildingCatalogSubsystem* Catalog = GetCatalog();
	if (!Catalog) return;

	const FBuildingEntry Entry = Catalog->GetBuilding(HeldBuilding->GetBuildingTypeId());
	if (Entry.BuildingTypeId.IsNone()) return;

	HeldModuleType = Entry.Data.ModuleType;
	bHeldGroundPlaceable = Entry.Data.bGroundPlaceable;
}

void UBuildComponent::Drop()
{
	if (!HeldBuilding) return;

	for (const FHeldMeshMaterials& Saved : HeldMaterials)
	{
		if (!Saved.Mesh) continue;

		for (int32 MaterialIndex = 0; MaterialIndex < Saved.Materials.Num(); MaterialIndex++)
		{
			Saved.Mesh->SetMaterial(MaterialIndex, Saved.Materials[MaterialIndex]);
		}
	}

	for (ABuilding* TreeBuilding : HeldTree)
	{
		if (TreeBuilding) TreeBuilding->SetActorEnableCollision(true);
	}

	HeldBuilding = nullptr;
	HeldTree.Reset();
	HeldMaterials.Reset();
	bHeldIsExisting = false;
	ClearCurrentSockets();
}

void UBuildComponent::ReleaseHeld()
{
	if (!HeldBuilding) return;

	if (bHeldIsExisting)
	{
		HeldBuilding->SetActorTransform(HeldOriginalTransform);
		Drop();
		return;
	}

	HeldBuilding->Destroy();
	HeldBuilding = nullptr;
	HeldTree.Reset();
	HeldMaterials.Reset();
	ClearCurrentSockets();
}

ABuilding* UBuildComponent::SpawnBuilding(TSubclassOf<ABuilding> BuildingClass, const FTransform& Transform, FName BuildingTypeId) const
{
	ABuilding* Building = GetWorld()->SpawnActorDeferred<ABuilding>(BuildingClass, Transform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!Building) return nullptr;

	Building->SetBuildingTypeId(BuildingTypeId);
	Building->FinishSpawning(Transform);
	return Building;
}

UBuildingCatalogSubsystem* UBuildComponent::GetCatalog() const
{
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (!GameInstance) return nullptr;

	return GameInstance->GetSubsystem<UBuildingCatalogSubsystem>();
}

void UBuildComponent::UpdateHeldLocation()
{
	if (!PlayerController) return;

	ClearCurrentSockets();

	FHitResult Hit;
	const bool bHasSurface = PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (!bHasSurface)
	{
		SetCanPlaceHeld(false);
		return;
	}

	ABuilding* TargetBuilding = Cast<ABuilding>(Hit.GetActor());

	if (TargetBuilding && TrySnapHeldToSocket(TargetBuilding, Hit.Location))
	{
		SetCanPlaceHeld(EvaluateCanPlaceHeld(true));
		return;
	}

	SnapHeldToGround(Hit.Location);

	if (TargetBuilding)
	{
		SetCanPlaceHeld(false);
		return;
	}

	SetCanPlaceHeld(EvaluateCanPlaceHeld(bHeldGroundPlaceable));
}

void UBuildComponent::GetHeldTreeBounds(FVector& Origin, FVector& Extent) const
{
	FBox Bounds(ForceInit);

	for (ABuilding* TreeBuilding : HeldTree)
	{
		if (!TreeBuilding) continue;

		Bounds += TreeBuilding->GetComponentsBoundingBox(true);
	}

	Bounds.GetCenterAndExtents(Origin, Extent);
}

void UBuildComponent::SnapHeldToGround(const FVector& GroundLocation)
{
	HeldBuilding->SetActorRotation(GetHeldRotation());
	HeldBuilding->SetActorLocation(GroundLocation);

	FVector Origin;
	FVector Extent;
	GetHeldTreeBounds(Origin, Extent);

	const float BottomOffset = GroundLocation.Z - (Origin.Z - Extent.Z);
	HeldBuilding->AddActorWorldOffset(FVector(0.f, 0.f, BottomOffset));
}

bool UBuildComponent::TrySnapHeldToSocket(ABuilding* TargetBuilding, const FVector& CursorLocation)
{
	UModuleSocketSceneComponent* TargetSocket = FindNearestAcceptingSocket(TargetBuilding, CursorLocation);
	if (!TargetSocket) return false;

	const FQuat BaseRotation = GetHeldRotation().Quaternion();
	HeldBuilding->SetActorRotation(BaseRotation);

	UModuleSocketSceneComponent* HeldSocket = FindHeldSocketFacing(TargetSocket);
	if (!HeldSocket) return false;

	const FVector DesiredDirection = -TargetSocket->GetOutwardDirection();
	const FQuat Correction = FQuat::FindBetweenNormals(HeldSocket->GetOutwardDirection(), DesiredDirection);
	HeldBuilding->SetActorRotation(Correction * BaseRotation);

	const FVector Offset = TargetSocket->GetComponentLocation() - HeldSocket->GetComponentLocation();
	HeldBuilding->AddActorWorldOffset(Offset);

	CurrentTargetSocket = TargetSocket;
	CurrentHeldBuildingSocket = HeldSocket;
	return true;
}

UModuleSocketSceneComponent* UBuildComponent::FindNearestAcceptingSocket(ABuilding* TargetBuilding, const FVector& CursorLocation) const
{
	TArray<UModuleSocketSceneComponent*> Sockets;
	TargetBuilding->GetComponents<UModuleSocketSceneComponent>(Sockets);

	UModuleSocketSceneComponent* NearestSocket = nullptr;
	float NearestDistanceSquared = TNumericLimits<float>::Max();

	for (UModuleSocketSceneComponent* Socket : Sockets)
	{
		if (!Socket->IsFree()) continue;
		if (!Socket->Accepts(HeldModuleType)) continue;

		const float DistanceSquared = FVector::DistSquared(Socket->GetComponentLocation(), CursorLocation);
		if (DistanceSquared >= NearestDistanceSquared) continue;

		NearestDistanceSquared = DistanceSquared;
		NearestSocket = Socket;
	}

	return NearestSocket;
}

UModuleSocketSceneComponent* UBuildComponent::FindHeldSocketFacing(const UModuleSocketSceneComponent* TargetSocket) const
{
	TArray<UModuleSocketSceneComponent*> Sockets;
	HeldBuilding->GetComponents<UModuleSocketSceneComponent>(Sockets);

	const FVector DesiredDirection = -TargetSocket->GetOutwardDirection();

	UModuleSocketSceneComponent* BestSocket = nullptr;
	float BestAlignment = -TNumericLimits<float>::Max();

	for (UModuleSocketSceneComponent* Socket : Sockets)
	{
		if (!Socket->IsFree()) continue;

		const float Alignment = FVector::DotProduct(Socket->GetOutwardDirection(), DesiredDirection);
		if (Alignment <= BestAlignment) continue;

		BestAlignment = Alignment;
		BestSocket = Socket;
	}

	return BestSocket;
}

void UBuildComponent::ClearCurrentSockets()
{
	CurrentTargetSocket = nullptr;
	CurrentHeldBuildingSocket = nullptr;
}

void UBuildComponent::ConnectBuilding(ABuilding* Building, UModuleSocketSceneComponent* TargetSocket, UModuleSocketSceneComponent* BuildingSocket)
{
	if (!Building) return;
	if (!TargetSocket) return;
	if (!BuildingSocket) return;

	Building->AttachToComponent(TargetSocket, FAttachmentTransformRules::KeepWorldTransform);
	TargetSocket->SetConnectedSocket(BuildingSocket);
}

bool UBuildComponent::EvaluateCanPlaceHeld(bool bSurfaceAllowed) const
{
	if (!bSurfaceAllowed) return false;
	if (IsHeldOverlapping()) return false;

	return true;
}

bool UBuildComponent::IsHeldOverlapping() const
{
	FVector Origin;
	FVector Extent;
	GetHeldTreeBounds(Origin, Extent);

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams;

	for (ABuilding* TreeBuilding : HeldTree)
	{
		QueryParams.AddIgnoredActor(TreeBuilding);
	}

	if (CurrentTargetSocket) QueryParams.AddIgnoredActor(CurrentTargetSocket->GetOwner());

	GetWorld()->OverlapMultiByObjectType(
		Overlaps,
		Origin,
		FQuat::Identity,
		FCollisionObjectQueryParams(FCollisionObjectQueryParams::AllObjects),
		FCollisionShape::MakeBox(Extent * OverlapBoundsScale),
		QueryParams
	);

	for (const FOverlapResult& Overlap : Overlaps)
	{
		if (Cast<ABuilding>(Overlap.GetActor())) return true;
	}

	return false;
}

void UBuildComponent::SetCanPlaceHeld(bool bNewValid)
{
	if (bCanPlaceHeld == bNewValid) return;

	bCanPlaceHeld = bNewValid;
	ApplyHeldMaterial();
	OnCanPlaceHeldChanged.Broadcast(bCanPlaceHeld);
}

void UBuildComponent::ApplyHeldMaterial() const
{
	if (!HeldBuilding) return;

	UMaterialInterface* Material = bCanPlaceHeld ? ValidHeldMaterial : InvalidHeldMaterial;
	if (!Material) return;

	for (const FHeldMeshMaterials& Saved : HeldMaterials)
	{
		if (!Saved.Mesh) continue;

		for (int32 MaterialIndex = 0; MaterialIndex < Saved.Materials.Num(); MaterialIndex++)
		{
			Saved.Mesh->SetMaterial(MaterialIndex, Material);
		}
	}
}

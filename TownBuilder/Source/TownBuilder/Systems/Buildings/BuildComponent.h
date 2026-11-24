#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Systems/Player/ToolComponent.h"
#include "BuildComponent.generated.h"

class ABuilding;
class APlayerController;
class UBuildingCatalogSubsystem;
class UMaterialInterface;
class UMeshComponent;
class UModuleSocketSceneComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoldStarted, FName, BuildingTypeId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHoldEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildingPlaced, ABuilding*, Building);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildingMoved, ABuilding*, Building);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCanPlaceHeldChanged, bool, bCanPlace);

USTRUCT()
struct FHeldMeshMaterials
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UMeshComponent> Mesh;

	UPROPERTY()
	TArray<TObjectPtr<UMaterialInterface>> Materials;
};

UCLASS(ClassGroup = (TownBuilder), meta = (BlueprintSpawnableComponent))
class TOWNBUILDER_API UBuildComponent : public UToolComponent
{
	GENERATED_BODY()


public:
	UBuildComponent();


	//Methods --> Placement
	UFUNCTION(BlueprintCallable, Category = "Build")
	void StartBuilding(FName BuildingTypeId);

	UFUNCTION(BlueprintCallable, Category = "Build")
	void StartMovingBuildingUnderCursor();

	UFUNCTION(BlueprintCallable, Category = "Build")
	void CancelHeld();

	UFUNCTION(BlueprintCallable, Category = "Build")
	ABuilding* PlaceHeld();

	UFUNCTION(BlueprintCallable, Category = "Build")
	void RotateHeld(float Direction);



	//Methods --> Getters
	UFUNCTION(BlueprintPure, Category = "Build")
	bool IsHolding() const { return HeldBuilding != nullptr; }

	UFUNCTION(BlueprintPure, Category = "Build")
	bool CanPlaceHeld() const { return bCanPlaceHeld; }


	//Events
	UPROPERTY(BlueprintAssignable, Category = "Build")
	FOnHoldStarted OnHoldStarted;

	UPROPERTY(BlueprintAssignable, Category = "Build")
	FOnHoldEnded OnHoldEnded;

	UPROPERTY(BlueprintAssignable, Category = "Build")
	FOnBuildingPlaced OnBuildingPlaced;

	UPROPERTY(BlueprintAssignable, Category = "Build")
	FOnBuildingMoved OnBuildingMoved;

	UPROPERTY(BlueprintAssignable, Category = "Build")
	FOnCanPlaceHeldChanged OnCanPlaceHeldChanged;


protected:
	//Setup & Tick
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	//Overrides
	virtual void ExitTool_Implementation() override;


	//Static Variables --> Editable
	UPROPERTY(EditDefaultsOnly, Category = "Build")
	TObjectPtr<UMaterialInterface> ValidHeldMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Build")
	TObjectPtr<UMaterialInterface> InvalidHeldMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	float RotationStep = 15.f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build", meta = (ClampMin = "0.5", ClampMax = "1.0"))
	float OverlapBoundsScale = 0.95f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	bool bKeepBuildingAfterPlace = true;


private:
	//Dynamic Variables
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY()
	TObjectPtr<ABuilding> HeldBuilding;

	UPROPERTY()
	TArray<TObjectPtr<ABuilding>> HeldTree;

	UPROPERTY()
	TArray<FHeldMeshMaterials> HeldMaterials;

	UPROPERTY()
	TObjectPtr<UModuleSocketSceneComponent> CurrentTargetSocket;

	UPROPERTY()
	TObjectPtr<UModuleSocketSceneComponent> CurrentHeldBuildingSocket;

	FTransform HeldOriginalTransform;
	FGameplayTag HeldModuleType;
	float HeldSpin = 0.f;
	bool bHeldGroundPlaceable = true;
	bool bHeldIsExisting = false;
	bool bCanPlaceHeld = false;


	//Methods --> Helpers
	// holding
	void Hold(ABuilding* Building, bool bIsExisting);
	void Drop();
	void ReleaseHeld();
	void CacheHeldPlacementRules();
	void ApplyHeldMaterial() const;
	void GetHeldTreeBounds(FVector& Origin, FVector& Extent) const;
	FRotator GetHeldRotation() const { return FRotator(0.f, HeldSpin, 0.f); }

	// snapping
	void UpdateHeldLocation();
	void SnapHeldToGround(const FVector& GroundLocation);
	bool TrySnapHeldToSocket(ABuilding* TargetBuilding, const FVector& CursorLocation);
	UModuleSocketSceneComponent* FindNearestAcceptingSocket(ABuilding* TargetBuilding, const FVector& CursorLocation) const;
	UModuleSocketSceneComponent* FindHeldSocketFacing(const UModuleSocketSceneComponent* TargetSocket) const;
	void ClearCurrentSockets();

	// connecting
	void ConnectBuilding(ABuilding* Building, UModuleSocketSceneComponent* TargetSocket, UModuleSocketSceneComponent* BuildingSocket);
	
	// validation
	bool EvaluateCanPlaceHeld(bool bSurfaceAllowed) const;
	bool IsHeldOverlapping() const;
	void SetCanPlaceHeld(bool bNewValid);
	
	// spawning
	ABuilding* SpawnBuilding(TSubclassOf<ABuilding> BuildingClass, const FTransform& Transform, FName BuildingTypeId) const;
	UBuildingCatalogSubsystem* GetCatalog() const;
};

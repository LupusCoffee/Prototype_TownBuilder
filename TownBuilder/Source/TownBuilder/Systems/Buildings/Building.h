#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Building.generated.h"

UCLASS()
class TOWNBUILDER_API ABuilding : public AActor
{
	GENERATED_BODY()


public:
	ABuilding();

	UFUNCTION(BlueprintCallable, Category = "Building")
	void SetBuildingTypeId(FName InBuildingTypeId);

	UFUNCTION(BlueprintPure, Category = "Building")
	FName GetBuildingTypeId() const { return BuildingTypeId; }


	//Methods --> Tree
	UFUNCTION(BlueprintPure, Category = "Building")
	ABuilding* GetRootBuilding();

	UFUNCTION(BlueprintPure, Category = "Building")
	TArray<ABuilding*> GetBuildingTree();


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
	FName BuildingTypeId;
};

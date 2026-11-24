#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Systems/Buildings/BuildingRow.h"
#include "BuildingCatalogSubsystem.generated.h"

class UDataTable;

UCLASS()
class TOWNBUILDER_API UBuildingCatalogSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()


public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintPure, Category = "Building Catalog")
	FBuildingEntry GetBuilding(FName BuildingTypeId) const;

	UFUNCTION(BlueprintPure, Category = "Building Catalog")
	TArray<FBuildingEntry> GetAllBuildings() const;


private:
	UPROPERTY()
	TObjectPtr<UDataTable> BuildingsTable;
};

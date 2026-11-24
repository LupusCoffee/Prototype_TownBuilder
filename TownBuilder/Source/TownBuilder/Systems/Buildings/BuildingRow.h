#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "BuildingRow.generated.h"

class ABuilding;
class UTexture2D;

USTRUCT(BlueprintType)
struct TOWNBUILDER_API FBuildingRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<UTexture2D> BuildWidgetIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Placement")
	TSubclassOf<ABuilding> ActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Placement", meta = (Categories = "Module"))
	FGameplayTag ModuleType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Placement")
	bool bIsFoundation = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Placement")
	bool bGroundPlaceable = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameValues")
	TMap<FName, int32> Cost;
};

USTRUCT(BlueprintType)
struct TOWNBUILDER_API FBuildingEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Building")
	FName BuildingTypeId;

	UPROPERTY(BlueprintReadOnly, Category = "Building")
	FBuildingRow Data;
};

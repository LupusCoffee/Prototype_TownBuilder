#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "TownBuilderSettings.generated.h"

class UDataTable;

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Town Builder"))
class TOWNBUILDER_API UTownBuilderSettings : public UDeveloperSettings
{
	GENERATED_BODY()


public:
	UTownBuilderSettings();

	UPROPERTY(Config, EditAnywhere, Category = "Data", meta = (RequiredAssetDataTags = "RowStructure=/Script/TownBuilder.BuildingRow"))
	TSoftObjectPtr<UDataTable> BuildingsTable;
};

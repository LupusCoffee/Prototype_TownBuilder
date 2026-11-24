#include "Systems/Buildings/BuildingCatalogSubsystem.h"
#include "Engine/DataTable.h"
#include "Systems/Core/TownBuilderSettings.h"

void UBuildingCatalogSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	BuildingsTable = GetDefault<UTownBuilderSettings>()->BuildingsTable.LoadSynchronous();

	if (!BuildingsTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("BuildingCatalogSubsystem: No buildings table in Project Settings > Game > Town Builder."));
	}
}

FBuildingEntry UBuildingCatalogSubsystem::GetBuilding(FName BuildingTypeId) const
{
	FBuildingEntry Entry;

	if (!BuildingsTable) return Entry;

	const FBuildingRow* Row = BuildingsTable->FindRow<FBuildingRow>(BuildingTypeId, TEXT("BuildingCatalogSubsystem"));
	if (!Row) return Entry;

	Entry.BuildingTypeId = BuildingTypeId;
	Entry.Data = *Row;
	return Entry;
}

TArray<FBuildingEntry> UBuildingCatalogSubsystem::GetAllBuildings() const
{
	TArray<FBuildingEntry> Entries;

	if (!BuildingsTable) return Entries;

	for (const FName& BuildingTypeId : BuildingsTable->GetRowNames())
	{
		Entries.Add(GetBuilding(BuildingTypeId));
	}

	return Entries;
}

#include "Systems/Buildings/DemolishComponent.h"
#include "GameFramework/PlayerController.h"
#include "Systems/Buildings/Building.h"
#include "Systems/Core/TraceLibrary.h"


void UDemolishComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<APlayerController>(GetOwner());
}


void UDemolishComponent::DemolishBuildingUnderCursor()
{
	ABuilding* Building = UTraceLibrary::GetActorUnderCursor<ABuilding>(PlayerController);
	if (!Building) return;

	OnBuildingDemolished.Broadcast(Building);
	Building->Destroy();
}

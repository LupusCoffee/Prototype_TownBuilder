#include "Systems/Buildings/Building.h"

ABuilding::ABuilding()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
}

void ABuilding::SetBuildingTypeId(FName InBuildingTypeId)
{
	BuildingTypeId = InBuildingTypeId;
}

ABuilding* ABuilding::GetRootBuilding()
{
	ABuilding* Root = this;
	ABuilding* Parent = Cast<ABuilding>(GetAttachParentActor());

	while (Parent)
	{
		Root = Parent;
		Parent = Cast<ABuilding>(Parent->GetAttachParentActor());
	}

	return Root;
}

TArray<ABuilding*> ABuilding::GetBuildingTree()
{
	TArray<ABuilding*> Tree;
	Tree.Add(this);

	for (int32 Index = 0; Index < Tree.Num(); Index++)
	{
		TArray<AActor*> AttachedActors;
		Tree[Index]->GetAttachedActors(AttachedActors, true, false);

		for (AActor* AttachedActor : AttachedActors)
		{
			ABuilding* AttachedBuilding = Cast<ABuilding>(AttachedActor);
			if (!AttachedBuilding) continue;

			Tree.AddUnique(AttachedBuilding);
		}
	}

	return Tree;
}

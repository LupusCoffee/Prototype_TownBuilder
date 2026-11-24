#pragma once

#include "CoreMinimal.h"
#include "Systems/Core/Components/ToolComponent.h"
#include "DemolishComponent.generated.h"

class ABuilding;
class APlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildingDemolished, ABuilding*, Building);

UCLASS(ClassGroup = (TownBuilder), meta = (BlueprintSpawnableComponent))
class TOWNBUILDER_API UDemolishComponent : public UToolComponent
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable, Category = "Demolish")
	void DemolishBuildingUnderCursor();

	
	UPROPERTY(BlueprintAssignable, Category = "Demolish")
	FOnBuildingDemolished OnBuildingDemolished;


protected:
	virtual void BeginPlay() override;


private:
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;
};

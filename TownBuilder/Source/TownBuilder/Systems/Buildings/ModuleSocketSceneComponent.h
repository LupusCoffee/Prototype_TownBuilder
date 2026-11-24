#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GameplayTagContainer.h"
#include "ModuleSocketSceneComponent.generated.h"

UCLASS(ClassGroup = (TownBuilder), meta = (BlueprintSpawnableComponent))
class TOWNBUILDER_API UModuleSocketSceneComponent : public USceneComponent
{
	GENERATED_BODY()


public:
	UModuleSocketSceneComponent();


	//Methods --> Socket
	UFUNCTION(BlueprintPure, Category = "Module Socket")
	bool Accepts(FGameplayTag ModuleType) const;

	UFUNCTION(BlueprintPure, Category = "Module Socket")
	bool IsFree() const { return ConnectedSocket == nullptr; }

	UFUNCTION(BlueprintCallable, Category = "Module Socket")
	void SetConnectedSocket(UModuleSocketSceneComponent* OtherSocket);

	UFUNCTION(BlueprintCallable, Category = "Module Socket")
	void ClearConnectedSocket();


	//Methods --> Getters
	UFUNCTION(BlueprintPure, Category = "Module Socket")
	UModuleSocketSceneComponent* GetConnectedSocket() const { return ConnectedSocket; }

	UFUNCTION(BlueprintPure, Category = "Module Socket")
	FVector GetOutwardDirection() const;


protected:
	//Overrides
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;


	//Static Variables --> Editable
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Module Socket", meta = (Categories = "Module"))
	FGameplayTagContainer AllowedModuleTypes;


private:
	//Dynamic Variables
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Module Socket")
	TObjectPtr<UModuleSocketSceneComponent> ConnectedSocket;
};

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Systems/Core/Enums/Tool.h"
#include "ToolComponent.generated.h"

UCLASS(Abstract, ClassGroup = (TownBuilder))
class TOWNBUILDER_API UToolComponent : public UActorComponent
{
	GENERATED_BODY()


public:
	//Methods --> Tool
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Tool")
	void EnterTool(ETool Tool);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Tool")
	void ExitTool();
};

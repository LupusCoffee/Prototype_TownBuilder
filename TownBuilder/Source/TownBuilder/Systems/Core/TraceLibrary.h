#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TraceLibrary.generated.h"

class APlayerController;

UCLASS()
class TOWNBUILDER_API UTraceLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


public:
	//Methods --> Tracing
	UFUNCTION(BlueprintCallable, Category = "Trace", meta = (DeterminesOutputType = "ActorClass"))
	static AActor* GetActorUnderCursor(const APlayerController* PlayerController, TSubclassOf<AActor> ActorClass);

	template <typename T>
	static T* GetActorUnderCursor(const APlayerController* PlayerController)
	{
		return Cast<T>(GetActorUnderCursor(PlayerController, T::StaticClass()));
	}
};

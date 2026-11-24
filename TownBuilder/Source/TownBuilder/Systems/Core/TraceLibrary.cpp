#include "Systems/Core/TraceLibrary.h"
#include "GameFramework/PlayerController.h"


AActor* UTraceLibrary::GetActorUnderCursor(const APlayerController* PlayerController, TSubclassOf<AActor> ActorClass)
{
	if (!PlayerController) return nullptr;
	if (!ActorClass) return nullptr;

	FHitResult Hit;
	if (!PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, Hit)) return nullptr;

	AActor* Actor = Hit.GetActor();
	if (!Actor) return nullptr;
	if (!Actor->IsA(ActorClass)) return nullptr;

	return Actor;
}

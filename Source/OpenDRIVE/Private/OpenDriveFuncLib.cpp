#include "OpenDriveFuncLib.h"

AActor* UOpenDriveFuncLib::CloneActor(AActor* InputActor) {
	if (!IsValid(InputActor)) return nullptr;
	UWorld* World = InputActor->GetWorld();
	FActorSpawnParameters params;
	params.Template = InputActor;

	UClass* ItemClass = InputActor->GetClass();
	AActor* const SpawnedActor = World->SpawnActor<AActor>(ItemClass, params);
	return SpawnedActor;
}

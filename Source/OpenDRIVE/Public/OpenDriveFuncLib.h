#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OpenDriveFuncLib.generated.h"

UCLASS()
class OPENDRIVE_API UOpenDriveFuncLib : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

	//TODO UE5: Use DuplicateActor from the Editor Actor Subsystem
	UFUNCTION(BlueprintCallable, Category = "ActorFuncions", meta = (DeterminesOutputType = "InputActor"))
	static AActor* CloneActor(AActor* InputActor);
};

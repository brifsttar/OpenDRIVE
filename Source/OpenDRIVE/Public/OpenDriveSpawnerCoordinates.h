#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "OpenDriveSpawnerCoordinates.generated.h"

UENUM(BlueprintType)
enum class ESnapOffsetTo : uint8 {
	NONE,
	OUTER,
	INNER,
	CENTER
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class OPENDRIVE_API UOpenDriveSpawnerCoordinates : public UAssetUserData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float S;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Lane;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESnapOffsetTo SnapOffsetTo;
	
};

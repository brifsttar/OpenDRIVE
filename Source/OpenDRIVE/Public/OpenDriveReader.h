#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OpenDriveReader.generated.h"

UCLASS()
class OPENDRIVE_API AOpenDriveReader : public AActor {
	GENERATED_BODY()

protected:
	UPROPERTY()
	class UTexture2D* SpriteTexture;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OpenDRIVE")
	class UOpenDriveComponent* OpenDrive;

	UPROPERTY(EditDefaultsOnly, Category = "OpenDRIVE")
	class UBillboardComponent* BillboardComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OpenDRIVE")
	int RoadId = -1;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OpenDRIVE")
	int LaneId = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OpenDRIVE")
	int JunctionId = -1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OpenDRIVE")
	float S = 0.;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OpenDRIVE")
	float T = 0.;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OpenDRIVE")
	float H = 0.;
	
public:
	AOpenDriveReader();

	void OnConstruction(const FTransform& Transform) override;

};

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"


class UBoxComponent;
class UPCGComponent;
class UPCGPointData;
struct FPCGPoint;


#include "SideWalkPCGSpawner.generated.h"

UCLASS()
class OPENDRIVE_API ASideWalkPCGSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASideWalkPCGSpawner();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPCGComponent* PCGComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PCG Settings", meta = (ClampMin = "10.0", ClampMax = "5000.0", UIMin = "10.0", UIMax = "5000.0"))
	float AverageSpacing = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PCG Settings", meta = (ClampMin = "0.0", ClampMax = "500.0", UIMin = "0.0", UIMax = "500.0"))
	float MaxOffset = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PCG Props Spacing", meta = (ClampMin = "1.0", ClampMax = "50.0", UIMin = "1.0", UIMax = "50.0"))
	float TreeSpacing = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PCG Props Spacing", meta = (ClampMin = "1.0", ClampMax = "50.0", UIMin = "1.0", UIMax = "50.0"))
	float LampSpacing = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PCG Props Spacing", meta = (ClampMin = "1.0", ClampMax = "50.0", UIMin = "1.0", UIMax = "50.0"))
	float BenchSpacing = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PCG Props Spacing", meta = (ClampMin = "-300.0", ClampMax = "300.0", UIMin = "-300.0", UIMax = "300.0"))
	float LampLateralOffset = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PCG Props Spacing", meta = (ClampMin = "0.0", ClampMax = "500.0", UIMin = "0.0", UIMax = "500.0"))
	float MarginBoxOffset = 50.0f;

	UPROPERTY(BlueprintReadOnly, Category = "PCG Data")
	TArray<FPCGPoint> GeneratedPCGPoints;

	UFUNCTION(BlueprintCallable, Category = "PCG Spawner")
	void SpawnPropsFromOpenDrive();

	UFUNCTION(BlueprintCallable, Category = "PCG Spawner")
	void ClearProps();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoadDecalSpawner.generated.h"

UCLASS()
class OPENDRIVE_API ARoadDecalSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoadDecalSpawner();

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* BoxComponent;

	UPROPERTY()
	TArray<class ADecalActor*> SpawnedDecals;

	UPROPERTY(EditAnywhere, Category = "Road Decal Spawner")
	TArray<class UMaterialInterface*> Materials;

	UPROPERTY(EditAnywhere, Category = "Road Decal Spawner")
	float SAverageSpacing = 2500.f;

	UPROPERTY(EditAnywhere, Category = "Road Decal Spawner")
	float TMaxOffset = 100.f;

	UPROPERTY(EditAnywhere, Category = "Road Decal Spawner")
	float ScaleRangeMin = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Road Decal Spawner")
	float ScaleRangeMax = 2.f;

protected:
	class UTexture2D* SpriteTexture;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, CallInEditor, meta = (Category = "Road Decal Spawner"))
	virtual void SpawnDecals();

	UFUNCTION(BlueprintCallable, CallInEditor, meta = (Category = "Road Decal Spawner"))
	virtual void ClearDecals();

};

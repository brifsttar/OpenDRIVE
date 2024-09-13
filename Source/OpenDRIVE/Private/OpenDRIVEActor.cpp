#include "OpenDRIVEActor.h"
#include "OpenDRIVE.h"
#include "Misc/UObjectToken.h"
#include "Logging/MessageLog.h"
#include "Roadmanager.hpp"
#include "Kismet/GameplayStatics.h" 
#if WITH_EDITOR
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "Subsystems/ImportSubsystem.h"
#endif

// Sets default values
AOpenDRIVEActor::AOpenDRIVEActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRootComponent")));
}

void AOpenDRIVEActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	LoadOpenDrive();
}

// Called when the game starts or when spawned
void AOpenDRIVEActor::BeginPlay()
{
	Super::BeginPlay();
	LoadOpenDrive();
}

void AOpenDRIVEActor::PostLoad()
{
	Super::PostLoad();
	LoadOpenDrive();
}

#if WITH_EDITOR 
void AOpenDRIVEActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	FName PropertyName = (PropertyChangedEvent.MemberProperty != NULL) ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOpenDRIVEActor, OpenDriveAsset))
	{
		LoadOpenDrive();
	}
}
#endif

void AOpenDRIVEActor::BeginDestroy()
{	
	TArray<AActor*> actors;
	bool bIsMultiple = CheckForMultipleOpenDRIVEActors(actors);
	OpenDRIVEError(bIsMultiple);
	if (bIsMultiple == false)
	{
		if (actors.IsEmpty() == false)
		{
			if (AOpenDRIVEActor* openDriveActor = Cast<AOpenDRIVEActor>(actors[0]))
			{
				openDriveActor->LoadOpenDrive();
			}
		}
	}
	Super::BeginDestroy();
}

void AOpenDRIVEActor::LoadOpenDrive()
{
	bool bIsMultiple = CheckForMultipleOpenDRIVEActors();
	OpenDRIVEError(bIsMultiple);
	if (bIsMultiple == true) return;
	if (!OpenDriveAsset) return;
	if (!roadmanager::Position::GetOpenDrive()->LoadOpenDriveContent(TCHAR_TO_UTF8(*(OpenDriveAsset->XodrContent)))) {
		UE_LOG(OpenDriveLog, Error, TEXT("Failed to load OpenDRIVE asset %s"), *(OpenDriveAsset->GetFName().ToString()));
		return;
	}
}

bool AOpenDRIVEActor::CheckForMultipleOpenDRIVEActors(TArray<AActor*>& actors)
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOpenDRIVEActor::StaticClass(), actors);
	return (actors.Num() > 1);
}

bool AOpenDRIVEActor::CheckForMultipleOpenDRIVEActors()
{
	TArray<AActor*> openDRIVEActorInScene;
	return (CheckForMultipleOpenDRIVEActors(openDRIVEActorInScene));
}

void AOpenDRIVEActor::OpenDRIVEError(bool bError)
{
	if (GetWorld())
	{
		if (UViewportStatsSubsystem* StatsSubsystem = GetWorld()->GetSubsystem<UViewportStatsSubsystem>())
		{
			FString Message("TOO MANY OPENDRIVEACTOR IN LEVEL. Please keep only OpenDRIVEActor in your scene.");

			StatsSubsystem->RemoveDisplayDelegate(_displayDelegateIdx);
			_displayDelegateIdx = StatsSubsystem->AddDisplayDelegate([this, Message, bError](FText& OutText, FLinearColor& OutColor)
				{
					OutText = FText::FromString(Message);
					OutColor = FLinearColor::Red;
					return IsValid(this) && bError;
				}
			);
		}
	}
}









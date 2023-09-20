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

// Called when the game starts or when spawned
void AOpenDRIVEActor::BeginPlay()
{
	Super::BeginPlay();
	
	CheckForMultipleOpenDRIVEActors() ? GEngine->AddOnScreenDebugMessage(0, 10.f, FColor::Red, TEXT("Multiple OpenDRIVEActor detected. Must be only OpenDRIVEActor per scene. \n The plugin can't work correctly."))
		: LoadOpenDrive();
}

#if WITH_EDITOR 
void AOpenDRIVEActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = (PropertyChangedEvent.MemberProperty != NULL) ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOpenDRIVEActor, OpenDriveAsset))
	{
		CheckForMultipleOpenDRIVEActors() ? GEngine->AddOnScreenDebugMessage(0, 10.f, FColor::Red, TEXT("Multiple OpenDRIVEActor detected. Must be only OpenDRIVEActor per scene. \n The plugin can't work correctly."))
			: LoadOpenDrive();
	}
}
#endif

void AOpenDRIVEActor::BeginDestroy()
{
	TArray<AActor*> actors;
	if (CheckForMultipleOpenDRIVEActors(actors) == false )
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
	if (!OpenDriveAsset) return;
	if (!roadmanager::Position::GetOpenDrive()->LoadOpenDriveContent(TCHAR_TO_UTF8(*(OpenDriveAsset->XodrContent)))) {
		UE_LOG(OpenDriveLog, Error, TEXT("Failed to load OpenDRIVE asset %s"), *(OpenDriveAsset->GetFName().ToString()));
		return;
	}
}

bool AOpenDRIVEActor::CheckForMultipleOpenDRIVEActors()
{
	TArray<AActor*> openDRIVEActorInScene;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOpenDRIVEActor::StaticClass(), openDRIVEActorInScene);

	return (openDRIVEActorInScene.Num() > 1);
}

bool AOpenDRIVEActor::CheckForMultipleOpenDRIVEActors(TArray<AActor*> &actors)
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOpenDRIVEActor::StaticClass(), actors);

	return (actors.Num() > 1);
}

#if WITH_EDITOR

void AOpenDRIVEActor::PostEditMove(bool bFinished)
{
	CheckForMultipleOpenDRIVEActors() ? GEngine->AddOnScreenDebugMessage(0, 10.f, FColor::Red, TEXT("Multiple OpenDRIVEActor detected. Must be only OpenDRIVEActor per scene. \n The plugin can't work correctly."))
		: LoadOpenDrive();

	Super::PostEditMove(bFinished);
}
#endif

void AOpenDRIVEActor::PostLoad()
{
	Super::PostLoad();

	CheckForMultipleOpenDRIVEActors() ? GEngine->AddOnScreenDebugMessage(0, 10.f, FColor::Red, TEXT("Multiple OpenDRIVEActor detected. Must be only OpenDRIVEActor per scene. \n The plugin can't work correctly."))
		: LoadOpenDrive();
}





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

}

// Called when the game starts or when spawned
void AOpenDRIVEActor::BeginPlay()
{
	Super::BeginPlay();
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

void AOpenDRIVEActor::LoadOpenDrive()
{
	if (!OpenDriveAsset) return;
	if (!roadmanager::Position::GetOpenDrive()->LoadOpenDriveContent(TCHAR_TO_UTF8(*(OpenDriveAsset->XodrContent)))) {
		UE_LOG(OpenDriveLog, Error, TEXT("Failed to load OpenDRIVE asset %s"), *(OpenDriveAsset->GetFName().ToString()));
		return;
	}
}

#if WITH_EDITOR

void AOpenDRIVEActor::PostEditMove(bool bFinished)
{
	TArray<AActor*> openDRIVEActorInScene;
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOpenDRIVEActor::StaticClass(), openDRIVEActorInScene);

	if (openDRIVEActorInScene.Num() > 1)
	{
		openDRIVEActorInScene[0]->Destroy();
	}
	
	Super::PostEditMove(bFinished);
}
#endif

void AOpenDRIVEActor::PostLoad()
{
	Super::PostLoad();

	LoadOpenDrive();
}





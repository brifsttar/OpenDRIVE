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

void AOpenDRIVEActor::CheckForErrors()
{
	Super::CheckForErrors();
	FMessageLog MapCheck("MapCheck");

	if (!GetLevel()) return;
	if (GetLevel()->IsPersistentLevel()) return;
	if (OpenDriveAsset) {
		MapCheck.Warning()
			->AddToken(FUObjectToken::Create(this))
			->AddToken(FTextToken::Create(FText::FromString("is a streamed level but has an OpenDRIVE set, its OpenDRIVE will be ignored")));
	}
}
#endif

void AOpenDRIVEActor::LoadOpenDrive()
{
	if (!OpenDriveAsset) return;
	if (GetLevel() && !GetLevel()->IsPersistentLevel()) {
		UE_LOG(OpenDriveLog, Warning, TEXT("%s is a streamed level but has an OpenDRIVE set, its OpenDRIVE will be ignored"), *(GetFName().ToString()));
		return;
	}
	if (!roadmanager::Position::GetOpenDrive()->LoadOpenDriveContent(TCHAR_TO_UTF8(*(OpenDriveAsset->XodrContent)))) {
		UE_LOG(OpenDriveLog, Error, TEXT("Failed to load OpenDRIVE asset %s"), *(OpenDriveAsset->GetFName().ToString()));
		return;
	}
#if WITH_EDITOR
	if (GEditor && !bRegisteredReimportCallback) {
		GEditor->GetEditorSubsystem<UImportSubsystem>()->OnAssetReimport.AddUObject(this, &AOpenDRIVEActor::OnObjectReimported);
		bRegisteredReimportCallback = true;
	}
#endif
}

#if WITH_EDITOR
void AOpenDRIVEActor::OnObjectReimported(UObject* InObject)
{
	if (InObject == OpenDriveAsset)
	{
		LoadOpenDrive();
	}
}

void AOpenDRIVEActor::PostEditMove(bool bFinished)
{
	TArray<AActor*> openDRIVEActorInScene;
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOpenDRIVEActor::StaticClass(), openDRIVEActorInScene);

	if (openDRIVEActorInScene.Num() > 1)
	{
		Destroy();
	}
	else
	{
		Super::PostEditMove(bFinished);
	}
}
#endif

void AOpenDRIVEActor::PostLoad()
{
	Super::PostLoad();
	TArray<AActor*> OpenDRIVEActors;

		LoadOpenDrive();
}





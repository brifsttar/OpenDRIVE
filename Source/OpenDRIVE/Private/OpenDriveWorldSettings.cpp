// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDriveWorldSettings.h"
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
#include "OpenDRIVEActor.h"

#if WITH_EDITOR
void AOpenDriveWorldSettings::PostEditChangeProperty(FPropertyChangedEvent& e) {
	Super::PostEditChangeProperty(e);

	FName PropertyName = (e.MemberProperty != NULL) ? e.MemberProperty->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOpenDriveWorldSettings, OpenDriveAsset)) {
		LoadOpenDrive();
	}
}

void AOpenDriveWorldSettings::CheckForErrors() {
	Super::CheckForErrors();
	FMessageLog MapCheck("MapCheck");

	if (HasOpenDriveActor() == false && IsValid(OpenDriveAsset))
	{
		MapCheck.Warning()->AddToken(FTextToken::Create(FText::FromString("OpenDriveWorldSettings are now deprecated. Use OpenDRIVEActor instead (see the readme.md on the plugin's repository for more info.")));
	}

	if (!GetLevel()) return;
	if (GetLevel()->IsPersistentLevel()) return;
	if (OpenDriveAsset) {
		MapCheck.Warning()
			->AddToken(FUObjectToken::Create(this))
			->AddToken(FTextToken::Create(FText::FromString("is a streamed level but has an OpenDRIVE set, its OpenDRIVE will be ignored")));
	}
}
#endif

void AOpenDriveWorldSettings::BeginPlay() {
	Super::BeginPlay();

	LoadOpenDrive();
}

void AOpenDriveWorldSettings::LoadOpenDrive() {
	if (!OpenDriveAsset) return;
	if (HasOpenDriveActor() == true) return;
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
		GEditor->GetEditorSubsystem<UImportSubsystem>()->OnAssetReimport.AddUObject(this, &AOpenDriveWorldSettings::OnObjectReimported);
		bRegisteredReimportCallback = true;
	}
#endif
}

void AOpenDriveWorldSettings::PostLoad() {
	Super::PostLoad();

	LoadOpenDrive();
}

bool AOpenDriveWorldSettings::HasOpenDriveActor()
{
	TArray<AActor*> actors;
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOpenDRIVEActor::StaticClass(), actors);
	
	return actors.Num() > 0;
}

#if WITH_EDITOR
void AOpenDriveWorldSettings::OnObjectReimported(UObject* InObject) {
	if (InObject == OpenDriveAsset) {
		LoadOpenDrive();
	}
}
#endif

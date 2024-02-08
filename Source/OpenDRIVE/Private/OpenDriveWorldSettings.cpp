// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDriveWorldSettings.h"
#include "OpenDRIVE.h"
#include "Misc/UObjectToken.h"
#include "Logging/MessageLog.h"
#include "OpenDRIVESubsystem.h"
#include "Roadmanager.hpp"
#if WITH_EDITOR
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "Subsystems/ImportSubsystem.h"
#endif

#if WITH_EDITOR
void AOpenDriveWorldSettings::PostEditChangeProperty(FPropertyChangedEvent& e) {
	Super::PostEditChangeProperty(e);

	FName PropertyName = (e.MemberProperty != NULL) ? e.MemberProperty->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOpenDriveWorldSettings, OpenDriveAsset)) {

		if (GetLevel() && !GetLevel()->IsPersistentLevel()) {
			UE_LOG(OpenDriveLog, Warning, TEXT("%s is a streamed level but has an OpenDRIVE set, its OpenDRIVE will be ignored"), *(GetFName().ToString()));
		}
		else 
		{
			GetWorld()->GetSubsystem<UOpenDRIVESubsystem>()->LoadOpenDrive(OpenDriveAsset);
		}
	}

#if WITH_EDITOR
	if (GEditor && !bRegisteredReimportCallback) {
		GEditor->GetEditorSubsystem<UImportSubsystem>()->OnAssetReimport.AddUObject(this, &AOpenDriveWorldSettings::OnObjectReimported);
		bRegisteredReimportCallback = true;
	}
#endif
}

void AOpenDriveWorldSettings::CheckForErrors() {
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

void AOpenDriveWorldSettings::OnObjectReimported(UObject* InObject)
{
	if (InObject == OpenDriveAsset) {
		GetWorld()->GetSubsystem<UOpenDRIVESubsystem>()->LoadOpenDrive(OpenDriveAsset);
	}
}

void AOpenDriveWorldSettings::PostLoad()
{
	Super::PostLoad();
	GetWorld()->GetSubsystem<UOpenDRIVESubsystem>()->LoadOpenDrive(OpenDriveAsset);
}

void AOpenDriveWorldSettings::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetSubsystem<UOpenDRIVESubsystem>()->LoadOpenDrive(OpenDriveAsset);
}
#endif

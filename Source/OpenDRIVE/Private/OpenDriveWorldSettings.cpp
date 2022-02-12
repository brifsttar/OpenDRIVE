// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDriveWorldSettings.h"
#include "Misc/UObjectToken.h"
#include "Roadmanager.hpp"

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
	if (GetLevel() && !GetLevel()->IsPersistentLevel()) {
		UE_LOG(OpenDriveLog, Warning, TEXT("%s is a streamed level but has an OpenDRIVE set, its OpenDRIVE will be ignored"), *(GetFName().ToString()));
	}
	if (!roadmanager::Position::GetOpenDrive()->LoadOpenDriveContent(TCHAR_TO_UTF8(*(OpenDriveAsset->XodrContent)))) {
		UE_LOG(OpenDriveLog, Error, TEXT("Failed to load OpenDRIVE asset %s"), *(OpenDriveAsset->GetFName().ToString()));
	}
}

void AOpenDriveWorldSettings::PostLoad() {
	Super::PostLoad();
	LoadOpenDrive();
}

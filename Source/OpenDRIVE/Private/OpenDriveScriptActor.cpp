// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDriveScriptActor.h"
#include "Misc/UObjectToken.h"
#include "Misc/MapErrors.h"
#include "Roadmanager.hpp"

DEFINE_LOG_CATEGORY(OpenDriveLog);

AOpenDriveScriptActor::AOpenDriveScriptActor() {
	PrimaryActorTick.bCanEverTick = true;
}

#if WITH_EDITOR
void AOpenDriveScriptActor::PostEditChangeProperty(FPropertyChangedEvent& e) {
	Super::PostEditChangeProperty(e);

	FName PropertyName = (e.MemberProperty != NULL) ? e.MemberProperty->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOpenDriveScriptActor, OpenDriveFile)) {
		FPaths::MakePathRelativeTo(OpenDriveFile.FilePath, *(FPaths::ProjectContentDir()));
		LoadOpenDrive();
	} else if (PropertyName == GET_MEMBER_NAME_CHECKED(AOpenDriveScriptActor, OpenDriveAsset)) {
		LoadOpenDrive();
	}
}

void AOpenDriveScriptActor::CheckForErrors() {
	Super::CheckForErrors();

	if (!GetLevel()) return;
	if (GetLevel()->IsPersistentLevel()) return;
	if (!OpenDriveFile.FilePath.IsEmpty() || OpenDriveAsset) {
		FMessageLog MapCheck("MapCheck");
		MapCheck.Warning()
			->AddToken(FUObjectToken::Create(this))
			->AddToken(FTextToken::Create(FText::FromString("is a streamed level but has an OpenDRIVE set, its OpenDRIVE will be ignored")));
	}
}
#endif

void AOpenDriveScriptActor::BeginPlay() {
	Super::BeginPlay();
	LoadOpenDrive();
}

void AOpenDriveScriptActor::LoadOpenDrive() {
	if (OpenDriveFile.FilePath.IsEmpty() && !OpenDriveAsset) return;
	if (GetLevel() && !GetLevel()->IsPersistentLevel()) {
		UE_LOG(OpenDriveLog, Warning, TEXT("%s is a streamed level but has an OpenDRIVE set, its OpenDRIVE will be ignored"), *(GetFName().ToString()));
	}
	if (OpenDriveAsset) {
		if (!roadmanager::Position::GetOpenDrive()->LoadOpenDriveContent(TCHAR_TO_UTF8(*(OpenDriveAsset->XodrContent)))) {
			UE_LOG(OpenDriveLog, Error, TEXT("Failed to load OpenDRIVE asset %s"), *(OpenDriveAsset->GetFName().ToString()));
		}
	} else {
		// Legacy file loading
		FString RelPath = FPaths::ProjectContentDir() + OpenDriveFile.FilePath;
		FString FullPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelPath);
		if (!roadmanager::Position::LoadOpenDrive(TCHAR_TO_UTF8(*(FullPath)))) {
			UE_LOG(OpenDriveLog, Error, TEXT("Failed to load OpenDRIVE file %s"), *RelPath);
		}
	}
}

void AOpenDriveScriptActor::PostLoad()
{
	Super::PostLoad();
	LoadOpenDrive();
}

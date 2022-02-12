// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDriveScriptActor.h"
#include "OpenDriveWorldSettings.h"
#include "Misc/UObjectToken.h"
#include "Misc/MapErrors.h"
#include "Roadmanager.hpp"

DEFINE_LOG_CATEGORY(OpenDriveLog);

#if WITH_EDITOR
void AOpenDriveScriptActor::CheckForErrors() {
	Super::CheckForErrors();
	FMessageLog MapCheck("MapCheck");

	//if (!Cast<AOpenDriveWorldSettings>(GetWorldSettings())) {
	//	MapCheck.Warning()
	//		->AddToken(FUObjectToken::Create(this))
	//		->AddToken(FTextToken::Create(FText::FromString("uses deprecated AOpenDriveScriptActor, please use AOpenDriveWorldSettings instead")));
	//}
	//MapCheck.Warning()
	//	->AddToken(FUObjectToken::Create(this))
	//	->AddToken(FTextToken::Create(FText::FromString("uses deprecated AOpenDriveScriptActor, please redirect it to ALevelScriptActor")));

	if (!GetLevel()) return;
	if (GetLevel()->IsPersistentLevel()) return;
	if (!OpenDriveFile.FilePath.IsEmpty() || OpenDriveAsset) {
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
	if (Cast<AOpenDriveWorldSettings>(GetWorldSettings())) {
		// Don't load if we're using the new World Settings class, it handles all that itself
		return;
	} else {
		UE_LOG(OpenDriveLog, Warning, TEXT("%s uses deprecated AOpenDriveScriptActor, please use AOpenDriveWorldSettings instead"), *(GetFName().ToString()));
	}
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
		UE_LOG(OpenDriveLog, Warning, TEXT("%s uses deprecated OpenDriveFile, use OpenDriveAsset instead"), *(GetFName().ToString()));
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

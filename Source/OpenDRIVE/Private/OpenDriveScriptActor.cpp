// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDriveScriptActor.h"
#include "Misc/UObjectToken.h"
#include "Misc/MapErrors.h"
#include "Roadmanager.hpp"

DEFINE_LOG_CATEGORY(OpenDriveLog);

AOpenDriveScriptActor::AOpenDriveScriptActor() {
	PrimaryActorTick.bCanEverTick = true;
}

roadmanager::OpenDrive *AOpenDriveScriptActor::GetOpenDrive() {
	return roadmanager::Position::GetOpenDrive();
}

#if WITH_EDITOR
void AOpenDriveScriptActor::PostEditChangeProperty(FPropertyChangedEvent& e) {
	Super::PostEditChangeProperty(e);

	FName PropertyName = (e.MemberProperty != NULL) ? e.MemberProperty->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AOpenDriveScriptActor, OpenDriveFile)) {
		FPaths::MakePathRelativeTo(OpenDriveFile.FilePath, *(FPaths::ProjectContentDir()));
		LoadOpenDrive();
	}
}

void AOpenDriveScriptActor::CheckForErrors() {
	Super::CheckForErrors();

	FMessageLog MapCheck("MapCheck");
	if (!OpenDriveFile.FilePath.IsEmpty() && GetLevel() && !GetLevel()->IsPersistentLevel()) {
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

void AOpenDriveScriptActor::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
}

void AOpenDriveScriptActor::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
}

void AOpenDriveScriptActor::LoadOpenDrive() {
	if (OpenDriveFile.FilePath.IsEmpty()) return;
	if (!GetLevel()) return;
	if (!GetLevel()->IsPersistentLevel()) {
		UE_LOG(OpenDriveLog, Warning, TEXT("%s is a streamed level but has an OpenDRIVE set, its OpenDRIVE will be ignored"), *(GetFName().ToString()));
	}
	FString RelPath = FPaths::ProjectContentDir() + OpenDriveFile.FilePath;
	FString FullPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelPath);
	if (!roadmanager::Position::LoadOpenDrive(TCHAR_TO_UTF8(*(FullPath)))) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Failed to load OpenDRIVE file %s"), *RelPath));
	}
}

void AOpenDriveScriptActor::PostLoad()
{
	Super::PostLoad();
	LoadOpenDrive();
}

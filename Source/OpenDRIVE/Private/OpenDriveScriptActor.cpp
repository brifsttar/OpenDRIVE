// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDriveScriptActor.h"
#include "OpenDriveWorldSettings.h"
#include "Misc/UObjectToken.h"
#include "Misc/MapErrors.h"
#include "Logging/MessageLog.h"
#include "Roadmanager.hpp"

DEFINE_LOG_CATEGORY(OpenDriveLog);

void AOpenDriveScriptActor::BeginPlay() {
	Super::BeginPlay();
	LoadOpenDrive();
}

void AOpenDriveScriptActor::LoadOpenDrive() {
	UE_LOG(OpenDriveLog, Warning, TEXT("%s uses deprecated AOpenDriveScriptActor, please use AOpenDriveWorldSettings instead"), *(GetFName().ToString()));
}

void AOpenDriveScriptActor::PostLoad() {
	Super::PostLoad();
	LoadOpenDrive();
}

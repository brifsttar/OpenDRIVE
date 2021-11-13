// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "UnrealEd.h"
#include "OpenDriveFactory.generated.h"

/**
 * 
 */
UCLASS(hidecategories = (Object))
class UOpenDriveFactory : public UFactory, public FReimportHandler
{
	GENERATED_UCLASS_BODY()

	virtual UObject *FactoryCreateFile(
		UClass *InClass,
		UObject *InParent,
		FName InName,
		EObjectFlags Flags,
		const FString &Filename,
		const TCHAR *Parms,
		FFeedbackContext *Warn,
		bool &bOutOperationCanceled
	) override;

	virtual bool CanReimport(UObject *Object, TArray<FString> &OutFilenames) override;
	virtual void SetReimportPaths(UObject *Object, const TArray<FString> &NewReimportPaths) override;
	virtual EReimportResult::Type Reimport(UObject *Object) override;

};

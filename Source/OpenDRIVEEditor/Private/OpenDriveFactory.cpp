// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDriveFactory.h"
#include "OpenDriveAsset.h"
#include "Misc/FileHelper.h"
#include "RoadManager.hpp"


UOpenDriveFactory::UOpenDriveFactory(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer) {
	Formats.Add(FString(TEXT("xodr;")) + NSLOCTEXT("UOpenDriveAssetFactory", "FormatXodr", "OpenDRIVE file").ToString());
	SupportedClass = UOpenDriveAsset::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
}

UObject *UOpenDriveFactory::FactoryCreateFile(
	UClass *InClass,
	UObject *InParent,
	FName InName,
	EObjectFlags Flags,
	const FString &Filename,
	const TCHAR *Parms,
	FFeedbackContext *Warn,
	bool &bOutOperationCanceled
) {
	UOpenDriveAsset *OdrAsset = nullptr;
	FString OdrContent;

	if (!FFileHelper::LoadFileToString(OdrContent, *Filename)) return nullptr;
	OdrAsset = NewObject<UOpenDriveAsset>(InParent, InClass, InName, Flags);
	OdrAsset->XodrContent = OdrContent;
	roadmanager::OpenDrive Odr;
	Odr.LoadOpenDriveContent(TCHAR_TO_UTF8(*(OdrContent)));
	OdrAsset->RoadCount = Odr.GetNumOfRoads();
	OdrAsset->AssetImportData->UpdateFilenameOnly(Filename);
	bOutOperationCanceled = false;

	return OdrAsset;
}
bool UOpenDriveFactory::CanReimport(UObject *Object, TArray<FString> &OutFilenames) {
	UOpenDriveAsset *OdrAsset = Cast<UOpenDriveAsset>(Object);
	if (!OdrAsset) return false;
	if (!OdrAsset->AssetImportData) return false;
	OdrAsset->AssetImportData->ExtractFilenames(OutFilenames);
	return true;
}

void UOpenDriveFactory::SetReimportPaths(UObject *Object, const TArray<FString> &NewReimportPaths) {
	UOpenDriveAsset *OdrAsset = Cast<UOpenDriveAsset>(Object);
	if (!OdrAsset) return;
	if (!OdrAsset->AssetImportData) return;
	if (!ensure(NewReimportPaths.Num() == 1)) return;
	OdrAsset->AssetImportData->UpdateFilenameOnly(NewReimportPaths[0]);
}

EReimportResult::Type UOpenDriveFactory::Reimport(UObject *Object) {
	UOpenDriveAsset *OdrAsset = Cast<UOpenDriveAsset>(Object);
	if (!OdrAsset) return EReimportResult::Failed;
	if (!OdrAsset->AssetImportData) return EReimportResult::Failed;
	bool bOperationCancelled = false;
	void *r = ImportObject(
		OdrAsset->GetClass(),
		OdrAsset->GetOuter(),
		*OdrAsset->GetName(),
		RF_Public | RF_Standalone,
		OdrAsset->AssetImportData->GetFirstFilename(),
		nullptr,
		bOperationCancelled
	);
	if (!r) return EReimportResult::Failed;
	return EReimportResult::Succeeded;
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "EnCardManagerLibrary.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Json.h"
#include "JsonUtilities/Public/JsonObjectConverter.h"
#include "GenericPlatform/GenericPlatformFile.h"



bool UEnCardManagerLibrary::MountLocalCard(FString CardId)
{
	IPlatformFile& InnerPlatform = FPlatformFileManager::Get().GetPlatformFile();
	FPakPlatformFile* PakPlatform = new FPakPlatformFile();
	PakPlatform->Initialize(&InnerPlatform, TEXT(""));
	FPlatformFileManager::Get().SetPlatformFile(*PakPlatform);

	FString PakFilePath = *UEnCardManagerLibrary::CardDirectory(CardId);
	FPakFile* PakFile = new FPakFile(&InnerPlatform, *PakFilePath, false);

	if (PakFile && PakFile->IsValid()) {
		GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, FString::Printf(TEXT("pak file: %s"), *PakFilePath));

		FString MountPoint = PakFile->GetMountPoint();
		FPaths::MakeStandardFilename(MountPoint);
		GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, FString::Printf(TEXT("mount point: %s"), *MountPoint));
		PakFile->SetMountPoint(*MountPoint);

		if (!PakPlatform->Mount(*PakFilePath, 0, *MountPoint)) {
			return false;
		}

		FString PluginContentDirectory = "../../../CreateEnCardTemplate/Plugins/EncardBuilder/Content/";
		FString PluginMountpoint = TEXT("/EncardBuilder/");
		FPaths::MakeStandardFilename(PluginContentDirectory);
		GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, FString::Printf(TEXT("plugin mount point: %s"), *PluginContentDirectory));
		FPackageName::RegisterMountPoint(PluginMountpoint, PluginContentDirectory);

		return true;
	}

	return false;
}

bool UEnCardManagerLibrary::UnMountLocalCard(FString CardId)
{
	IPlatformFile& InnerPlatform = FPlatformFileManager::Get().GetPlatformFile();
	FPakPlatformFile* PakPlatform = new FPakPlatformFile();
	PakPlatform->Initialize(&InnerPlatform, TEXT(""));
	FPlatformFileManager::Get().SetPlatformFile(*PakPlatform);

	FString PakFilePath = *UEnCardManagerLibrary::CardDirectory(CardId);
	FPakFile* PakFile = new FPakFile(&InnerPlatform, *PakFilePath, false);

	if (PakFile && PakFile->IsValid()) {
		GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, FString::Printf(TEXT("pak file: %s"), *PakFilePath));

		FString MountPoint = PakFile->GetMountPoint();
		FPaths::MakeStandardFilename(MountPoint);
		GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, FString::Printf(TEXT("mount point: %s"), *MountPoint));
		PakFile->SetMountPoint(*MountPoint);

		if (!PakPlatform->Unmount(*PakFilePath)) {
			return false;
		}

		return true;
	}

	return false;
}

bool UEnCardManagerLibrary::AddNewLocalCard(UPARAM(ref)FLocalCard& NewCardData)
{
	TArray<FLocalCard>& Cards = UEnCardManagerLibrary::ReadLocalCards().LocalCards;

	// Maximun local Spheres is 10 spheres
	if (Cards.Num() > 10) {
		const FLocalCard& LastCard = Cards[0];
		FString LastCardPath;

		bool isExists = UEnCardManagerLibrary::LocalCardExists(LastCard.id, LastCardPath);

		if (isExists) {
			IFileManager& FileManager = IFileManager::Get();
			FileManager.Delete(*LastCardPath);
		}

		// Remove from LocalSpheres
		Cards.RemoveAt(0);
	}

	Cards.Add(NewCardData);

	// Update info json file
	return	UEnCardManagerLibrary::WriteLocalCards(LocalCards);;
}

bool UEnCardManagerLibrary::LocalCardExists(FString CardId, FString& OutCardPath)
{
	// Check if id is !Empty
	if (CardId.IsEmpty()) { return false; }

	// Validate Spehre .PAK file
	OutCardPath = UEnCardManagerLibrary::CardDirectory(CardId);
	return FPaths::ValidatePath(OutCardPath) && FPaths::FileExists(OutCardPath);
}

FString UEnCardManagerLibrary::CardDirectory(FString CardId)
{
	FString CardFolder = FPaths::ProjectDir() + "TempCard";
	return CardFolder + "/" + CardId + ".pak";
}

FLocalCards& UEnCardManagerLibrary::ReadLocalCards()
{
	if (InitedInfo) { return LocalCards; }

	// Get Cards infomation directory
	FString CardFolder = FPaths::ProjectDir() + "TempCard";
	FString LocalCardFile = CardFolder + "/LocalCards.json";

	if (FPaths::FileExists(LocalCardFile)) {

		// Load Json from LocalCardFile
		FString LocalCardsString;
		FFileHelper::LoadFileToString(LocalCardsString, *LocalCardFile);

		// Convert Json String to SphereInfo struct
		FJsonObjectConverter::JsonObjectStringToUStruct(LocalCardsString, &LocalCards, 0, 0);
	}
	else {
		UEnCardManagerLibrary::WriteLocalCards(LocalCards);
	}

	// Set Inited to true
	InitedInfo = true;

	return LocalCards;
}

bool UEnCardManagerLibrary::WriteLocalCards(const FLocalCards& Cards)
{
	// Get Cards infomation directory
	FString CardFolder = FPaths::ProjectDir() + "TempCard";
	FString LocalCardFile = CardFolder + "/LocalCards.json";

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*CardFolder))
		PlatformFile.CreateDirectory(*CardFolder);

	// Convert FLocalCards struct to String
	FString OutputString;
	FJsonObjectConverter::UStructToJsonObjectString(Cards, OutputString);

	// Save to local dir
	return FFileHelper::SaveStringToFile(OutputString, *LocalCardFile, FFileHelper::EEncodingOptions::ForceUTF8);;
}

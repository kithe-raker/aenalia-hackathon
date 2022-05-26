// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "IPlatformFilePak.h"
#include "EnCardManagerLibrary.generated.h"

USTRUCT(BlueprintType)
struct FLocalCard
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Local Encard")
	FString id;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Local Encard")
	FString pak_url;

};

USTRUCT()
struct FLocalCards
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FLocalCard> LocalCards = TArray<FLocalCard>();
};

UCLASS(BlueprintType)
class AENALIAFOUNDATION_API UEnCardManagerLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, Category = "Encard Manager")
	bool MountLocalCard(FString CardId);

	UFUNCTION(BlueprintCallable, Category = "Encard Manager")
	bool UnMountLocalCard(FString CardId);

	UFUNCTION(BlueprintCallable, Category = "Encard Manager")
	bool AddNewLocalCard(UPARAM(ref) FLocalCard& NewCardData);

	UFUNCTION(BlueprintCallable, Category = "Encard Manager")
	static bool LocalCardExists(FString CardId, FString& OutCardPath);

	UFUNCTION(BlueprintCallable, Category = "Encard Manager")
	static FString CardDirectory(FString CardId);

	FLocalCards& ReadLocalCards();

	static bool WriteLocalCards(const FLocalCards& Cards);


private:
	bool InitedInfo = false;
	FLocalCards LocalCards = FLocalCards();
};

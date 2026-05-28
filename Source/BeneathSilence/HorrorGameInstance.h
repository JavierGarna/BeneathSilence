// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "HorrorGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BENEATHSILENCE_API UHorrorGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	int ClaimScan();
	UFUNCTION(BlueprintCallable)
	void RemoveScan(int ScanID);
	
private:
	static constexpr int MAXSCANS = 6;
	int Scans[MAXSCANS] = { -1, -1, -1, -1, -1, -1 };
};

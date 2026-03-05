// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainGameModeBase.generated.h"

class UUserWidget;

/**
 * 
 */
UCLASS()
class BENEATHSILENCE_API AMainGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMainGameModeBase();

	UFUNCTION()
	void CollectItem();

	UFUNCTION(BlueprintPure)
	int GetItemsToCollect();

	UFUNCTION(BlueprintPure)
	int GetCollectedItems();

	UFUNCTION(BlueprintPure)
	bool GetHasWon();

	UFUNCTION(BlueprintPure)
	bool GetHasLost();

	UFUNCTION()
	void GameOver(bool gameOver);

private:

	UPROPERTY(EditAnywhere)
	int itemsToCollect = 5;

	UPROPERTY(EditAnywhere)
	int collectedItems = 0;

	UPROPERTY(EditAnywhere)
	bool hasWon = false;

	UPROPERTY(EditAnywhere)
	bool hasLost = false;
};

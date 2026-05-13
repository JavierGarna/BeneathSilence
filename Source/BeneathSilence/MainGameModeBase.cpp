// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameModeBase.h"
#include "MainPlayerController.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

AMainGameModeBase::AMainGameModeBase()
{
	// Set default player controller class to your custom player controller
	PlayerControllerClass = AMainPlayerController::StaticClass();

	// Set default pawn class to your custom player character
	DefaultPawnClass = APlayerCharacter::StaticClass();
}

void AMainGameModeBase::CollectItem()
{
	collectedItems++;

	if (collectedItems >= itemsToCollect)
	{
		GameOver(true);
	}
}

int AMainGameModeBase::GetItemsToCollect()
{
	return itemsToCollect;
}

int AMainGameModeBase::GetCollectedItems()
{
	return collectedItems;
}

bool AMainGameModeBase::GetHasWon()
{
	return hasWon;
}

bool AMainGameModeBase::GetHasLost()
{
	return hasLost;
}

void AMainGameModeBase::GameOver(bool gameOver)
{
	if (gameOver)
	{
		hasWon = true;
	}
	else
	{
		hasLost = true;
	}

	// Pause the game
	//UGameplayStatics::SetGamePaused(GetWorld(), true);
}
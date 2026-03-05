// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameModeBase.h"
#include "MainPlayerController.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

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

void AMainGameModeBase::StartGame()
{
}

void AMainGameModeBase::GameOver(bool gameOver)
{
	UE_LOG(LogTemp, Display, TEXT("Game Over"));
}

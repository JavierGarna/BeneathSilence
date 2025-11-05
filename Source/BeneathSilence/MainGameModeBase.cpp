// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameModeBase.h"
#include "MainPlayerController.h"
#include "PlayerCharacter.h"

AMainGameModeBase::AMainGameModeBase()
{
	// Set default player controller class to your custom player controller
	PlayerControllerClass = AMainPlayerController::StaticClass();

	// Set default pawn class to your custom player character
	DefaultPawnClass = APlayerCharacter::StaticClass();
}

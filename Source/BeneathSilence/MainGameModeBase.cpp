// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameModeBase.h"
#include "MainPlayerController.h"


AMainGameModeBase::AMainGameModeBase()
{
	PlayerControllerClass = AMainPlayerController::StaticClass();
}

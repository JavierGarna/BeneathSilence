// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "FixedCameraManager.h"

AMainPlayerController::AMainPlayerController()
{
	PlayerCameraManagerClass = AFixedCameraManager::StaticClass();
}

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

}

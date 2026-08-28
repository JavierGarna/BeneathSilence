// Fill out your copyright notice in the Description page of Project Settings.


#include "WinVolume.h"
#include "MainGameModeBase.h"
#include "Kismet/GameplayStatics.h"

AWinVolume::AWinVolume()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	OnActorBeginOverlap.AddDynamic(this, &AWinVolume::HandleBeginOverlap);
}

void AWinVolume::HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	AMainGameModeBase* GameMode = GetWorld() ? Cast<AMainGameModeBase>(GetWorld()->GetAuthGameMode()) : nullptr;
	if (GameMode)
	{
		GameMode->WinGame();
	}
}

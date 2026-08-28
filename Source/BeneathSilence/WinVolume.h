// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "WinVolume.generated.h"

class UBoxComponent;

/**
 * 
 */
UCLASS()
class BENEATHSILENCE_API AWinVolume : public ATriggerVolume
{
	GENERATED_BODY()

public:
	AWinVolume();

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* WinTrigger;

protected:
	UFUNCTION()
	void HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	
};

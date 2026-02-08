// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "RoomVolume.generated.h"

/**
 * 
 */
UCLASS()
class BENEATHSILENCE_API ARoomVolume : public ATriggerVolume
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RoomName;
};

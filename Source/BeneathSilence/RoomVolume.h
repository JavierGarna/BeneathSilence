// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "RoomVolume.generated.h"

class AFixedCamera;
class UBoxComponent;

/**
 * 
 */
UCLASS()
class BENEATHSILENCE_API ARoomVolume : public ATriggerVolume
{
	GENERATED_BODY()

public:
	ARoomVolume();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RoomName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ARoomVolume*> ConnectedRooms;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AFixedCamera* RoomCamera;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* RoomTrigger;

	UPROPERTY(EditAnywhere)
	float PlayerTimeInRoom = 0.f;

	UPROPERTY(EditAnywhere)
	float EnemyTimeInRoom = 0.f;

protected:
	UFUNCTION()
	void HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SoundLight.generated.h"

USTRUCT()
struct FSoundWaveInstance
{
	GENERATED_BODY()

	FTimerHandle TimerHandle;
	int32 CurrentStep = 0;
	int32 AnimationSteps = 32;

	FVector StartPoint;
	float Radius = 0.f;
	int RayCount = 0;
	float Speed = 0.f;
};

UCLASS()
class BENEATHSILENCE_API ASoundLight : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASoundLight();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void StartSoundWave(const FVector& Origin, float Radius, int RayCount, float Speed);

private:
	float ZOffset = 5.f;

	UPROPERTY()
	TArray<FSoundWaveInstance> ActiveWaves;

	UFUNCTION()
	void TickWave(int WaveIndex);
	UFUNCTION()
	void AnimateCurrentWave(const FVector& StartPoint, float Radius, int RayCount, float VerticalAngle);
};

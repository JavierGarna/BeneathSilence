// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SoundLight.generated.h"

class AResidualDecal;

USTRUCT()
struct FSoundWaveInstance
{
	GENERATED_BODY()

	FTimerHandle TimerHandle;
	int CurrentStep = 0;
	int AnimationSteps = 32;

	int ID = INDEX_NONE;
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
	int NextWaveID = 1;

	UPROPERTY()
	TArray<FSoundWaveInstance> ActiveWaves;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AResidualDecal> ResidualDecalClass;

	UFUNCTION()
	void TickWave(int WaveID);
	UFUNCTION()
	void AnimateCurrentWave(const FVector& StartPoint, float Radius, int RayCount, float VerticalAngle);
};

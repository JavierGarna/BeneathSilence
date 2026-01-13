// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ResidualDecal.generated.h"

UCLASS()
class BENEATHSILENCE_API AResidualDecal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AResidualDecal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere)
	UDecalComponent* DecalComponent;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* DecalMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* MID;

	UPROPERTY(EditAnywhere)
	float ElapsedTime = 0.f;
	UPROPERTY(EditAnywhere)
	float Intensity = 0.8f;
	UPROPERTY(EditAnywhere)
	float TotalLifetime = 1.0f;
	UPROPERTY(EditAnywhere)
	float FadeCurve = 20.0f;
};

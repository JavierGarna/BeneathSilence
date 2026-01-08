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

	float ElapsedTime = 0.f;

	UPROPERTY(EditAnywhere)
	float LifeTime = 1.5f;

	UPROPERTY(EditDefaultsOnly)
	float InitialIntensity = 0.2f;
};

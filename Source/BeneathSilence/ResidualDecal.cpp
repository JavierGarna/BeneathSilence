// Fill out your copyright notice in the Description page of Project Settings.


#include "ResidualDecal.h"
#include "Components/DecalComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
AResidualDecal::AResidualDecal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	RootComponent = DecalComponent;

	DecalComponent->DecalSize = FVector(6.f, 16.f, 16.f);
}

// Called when the game starts or when spawned
void AResidualDecal::BeginPlay()
{
	Super::BeginPlay();
	
	if (!DecalMaterial)
	{
		UE_LOG(LogTemp, Warning, TEXT("ResidualDecal: No DecalMaterial assigned"));
		return;
	}

	DecalComponent->SetDecalMaterial(DecalMaterial);

	MID = DecalComponent->CreateDynamicMaterialInstance();

	if (MID)
	{
		MID->SetScalarParameterValue(TEXT("Intensity"), InitialIntensity);
	}
}

// Called every frame
void AResidualDecal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime;
	float Alpha = FMath::Clamp(ElapsedTime / LifeTime, 0.f, 1.f);

	if (MID)
	{
		MID->SetScalarParameterValue(TEXT("Intensity"), FMath::Lerp(InitialIntensity, 0.f, Alpha));
	}

	if (Alpha >= 1.f)
	{
		Destroy();
	}
}


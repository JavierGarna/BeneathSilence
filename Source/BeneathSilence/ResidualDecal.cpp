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
}

// Called when the game starts or when spawned
void AResidualDecal::BeginPlay()
{
	Super::BeginPlay();
	
	if (!DecalMaterial) return;

	DecalComponent->SetDecalMaterial(DecalMaterial);
	MID = DecalComponent->CreateDynamicMaterialInstance();
}

// Called every frame
void AResidualDecal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    ElapsedTime += DeltaTime;

    if (MID)
    {
        float CurrentIntensity;

        // Slow residual fade: residual -> 0
        float FadeAlpha = ElapsedTime / TotalLifetime;
        FadeAlpha = FMath::Clamp(FadeAlpha, 0.f, 1.f);

        // Apply exponential decay for gradual dissipation
        float EasedAlpha = FMath::Pow(FadeAlpha, FadeCurve);
        CurrentIntensity = FMath::Lerp(Intensity, 0.f, EasedAlpha);

        MID->SetScalarParameterValue(TEXT("Intensity"), CurrentIntensity);
    }

    if (ElapsedTime >= TotalLifetime) Destroy();
}


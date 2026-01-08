// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundLight.h"
#include "ResidualDecal.h"

// Sets default values
ASoundLight::ASoundLight()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASoundLight::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASoundLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
 
void ASoundLight::StartSoundWave(const FVector& Origin, float Radius, int RayCount, float Speed)
{
	// Prevent invalid parameters
    if (Speed <= 0.f) return;
    if (Radius <= 0.f) return;

	// Get the world context
    UWorld* World = GetWorld();
    if (!World) return;

	// Create a new sound wave instance
	FSoundWaveInstance& Wave = ActiveWaves.AddDefaulted_GetRef(); // Adds new defaulted instance and returns a reference
	// Assign wave properties
    Wave.ID = NextWaveID++;
    Wave.StartPoint = Origin + FVector::UpVector * ZOffset;
    Wave.Radius = Radius;
    Wave.RayCount = RayCount;
	Wave.Speed = Speed;

	// Set up a timer to tick the wave animation. Every Wave.Speed seconds, TickWave will be called with Wave.ID
    World->GetTimerManager().SetTimer(
        Wave.TimerHandle,
        FTimerDelegate::CreateUObject(
            this,
            &ASoundLight::TickWave,
            Wave.ID
        ),
        Wave.Speed,
        true
    );
}

void ASoundLight::TickWave(int WaveID)
{
	int32 WaveIndex = ActiveWaves.IndexOfByPredicate([WaveID](const FSoundWaveInstance& W) { return W.ID == WaveID; });
    
	// Return if wave does not exist
    if (!ActiveWaves.IsValidIndex(WaveIndex))
        return;

    FSoundWaveInstance& Wave = ActiveWaves[WaveIndex];
    if (Wave.CurrentStep > Wave.AnimationSteps) // When the wave has reached the last animation step
    {
        GetWorld()->GetTimerManager().ClearTimer(Wave.TimerHandle);
        ActiveWaves.RemoveAt(WaveIndex);
        return;
    }

    float Progress = Wave.CurrentStep / static_cast<float>(Wave.AnimationSteps);

    float VerticalAngle = FMath::Lerp(-90.f, 90.f, Progress);

    AnimateCurrentWave(
        Wave.StartPoint,
        Wave.Radius,
        Wave.RayCount,
        VerticalAngle
    );

    Wave.CurrentStep++;
}

void ASoundLight::AnimateCurrentWave(const FVector& StartPoint, float Radius, int RayCount, float VerticalAngle)
{
    UWorld* World = GetWorld();
    if (!World)
        return;

    for (int i = 0; i < RayCount; ++i)
    {
        float HorizontalAngle = (360.f / RayCount) * i;
        FVector Direction = FRotator(VerticalAngle, HorizontalAngle, 0.f).Vector();
        FVector EndPoint = StartPoint + Direction * Radius;

        FHitResult Hit;
        FCollisionQueryParams Params;
        Params.bTraceComplex = false;
        Params.AddIgnoredActor(this);

        bool bHit = World->LineTraceSingleByChannel(
            Hit, 
            StartPoint,
            EndPoint,
            ECC_Visibility,
            Params
        );

        if (bHit)
        {
			FRotator DecalRotation = Hit.ImpactNormal.Rotation();

            World->SpawnActor<AResidualDecal>(
                ResidualDecalClass,
                Hit.ImpactPoint,
                DecalRotation
			);
        }
    }
}


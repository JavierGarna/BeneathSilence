// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundLight.h"

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
	// Get the world context
    UWorld* World = GetWorld();
    if (!World) return;

	// Create a new sound wave instance
    FSoundWaveInstance& Wave = ActiveWaves.AddDefaulted_GetRef();

	// Initialise wave parameters
    Wave.StartPoint = Origin + FVector::UpVector * ZOffset;
    Wave.Radius = Radius;
    Wave.RayCount = RayCount;
	Wave.Speed = Speed;

	// Set up a timer to tick the wave animation
    World->GetTimerManager().SetTimer(
        Wave.TimerHandle,
        FTimerDelegate::CreateUObject(
            this,
            &ASoundLight::TickWave,
            ActiveWaves.Num() - 1 // Wave index
        ),
        Wave.Speed,
        true
    );
}

void ASoundLight::TickWave(int WaveIndex)
{
    if (!ActiveWaves.IsValidIndex(WaveIndex))
        return;

    FSoundWaveInstance& Wave = ActiveWaves[WaveIndex];

    if (Wave.CurrentStep > Wave.AnimationSteps)
    {
        GetWorld()->GetTimerManager().ClearTimer(Wave.TimerHandle);
        ActiveWaves.RemoveAt(WaveIndex);
        return;
    }

    float Progress =
        Wave.CurrentStep / static_cast<float>(Wave.AnimationSteps);

    float VerticalAngle =
        FMath::Lerp(-90.f, 90.f, Progress);

    AnimateCurrentWave(
        Wave.StartPoint,
        Wave.Radius,
        Wave.RayCount,
        VerticalAngle
    );

    ++Wave.CurrentStep;
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

        FVector DrawEnd = bHit ? Hit.ImpactPoint : EndPoint;

        DrawDebugLine(
            World,
            StartPoint,
            DrawEnd,
            FColor::Cyan,
            false,
            0.2f,
            0,
            2.f
        );
    }
}


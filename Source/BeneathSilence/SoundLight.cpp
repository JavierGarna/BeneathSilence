// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundLight.h"
#include "ResidualDecal.h"
#include "Perception/AISense_Hearing.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

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
 
void ASoundLight::StartSoundWave(const FVector& Origin, float Radius, int RayCount, float Speed, AActor* WaveOwner)
{
	// Prevent invalid parameters
    if (Radius <= 0.f || Speed <= 0.f || RayCount <= 0) return;

	// Get the world context
    UWorld* World = GetWorld();
    if (!World) return;

	// Report the noise event to the AI perception system so that AI can react to it
    UAISense_Hearing::ReportNoiseEvent(
        GetWorld(),
        Origin,
        100.f,
        WaveOwner,
        Radius,
        NAME_None
    );

    WaveOwnerActor = WaveOwner;

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
	int32 WaveIndex = ActiveWaves.IndexOfByPredicate([WaveID](const FSoundWaveInstance& W) { return W.ID == WaveID; }); // Find index of the wave with matching ID
    
	// Return if wave does not exist
    if (!ActiveWaves.IsValidIndex(WaveIndex))
        return;

	FSoundWaveInstance& Wave = ActiveWaves[WaveIndex]; // Get reference to the wave instance

	if (Wave.CurrentStep > Wave.AnimationSteps) // Clear timer and remove wave when animation is complete
    {
        GetWorld()->GetTimerManager().ClearTimer(Wave.TimerHandle);
        ActiveWaves.RemoveAt(WaveIndex);
        return;
    }

	float Progress = Wave.CurrentStep / static_cast<float>(Wave.AnimationSteps); // Calculate progress (0 to 1)
	float CurrentRadius = FMath::Lerp(Wave.Radius, 0.f, Progress); // Interpolate current radius from max radius to 0

	float VerticalAngle = FMath::Lerp(-90.f, 90.f, Progress); // Interpolate vertical angle from -90 to 90 degrees

	// Call animation function
    AnimateCurrentWave(
        Wave.StartPoint,
        CurrentRadius,
        Wave.RayCount,
        VerticalAngle
    );

	Wave.CurrentStep++; // Increment step
}

void ASoundLight::AnimateCurrentWave(const FVector& StartPoint, float Radius, int RayCount, float VerticalAngle)
{
    UWorld* World = GetWorld();
    if (!World)
        return;

    for (int i = 0; i < RayCount; ++i)
    {
		float HorizontalAngle = (360.f / RayCount) * i; // Distribute rays evenly in horizontal plane
		FVector Direction = FRotator(VerticalAngle, HorizontalAngle, 0.f).Vector(); // Calculate direction vector with vertical and horizontal angles
		FVector EndPoint = StartPoint + Direction * Radius; // Calculate end point of the ray

        FHitResult Hit;
        FCollisionQueryParams Params;
        Params.bTraceComplex = false;
        Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(WaveOwnerActor);

        bool bHit = World->LineTraceSingleByChannel(
            Hit, 
            StartPoint,
            EndPoint,
            ECC_Pawn,
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

            AActor* HitActor = Hit.GetActor();
            if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(HitActor))
            {
                // Get Enemy Controller and make it aware of the noise
                if (AEnemyAIController* EnemyController = Cast<AEnemyAIController>(Enemy->GetController()))
                {
					EnemyController->GetBlackboardComponent()->SetValueAsBool("HasHeardPlayer", true);
				}
			}
            
        }
    }
}


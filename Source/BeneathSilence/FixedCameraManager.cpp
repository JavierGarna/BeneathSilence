// Fill out your copyright notice in the Description page of Project Settings.


#include "FixedCameraManager.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

AFixedCameraManager::AFixedCameraManager()
{
    CurrentFixedCamera = nullptr;
    TargetFixedCamera = nullptr;
    BlendAlpha = 0.0f;
    BlendDuration = 0.5f;
    BlendTimeElapsed = 0.0f;
    bIsBlending = false;
}

void AFixedCameraManager::SwitchToFixedCamera(AActor* NewCameraActor, float BlendTime)
{
    if (!NewCameraActor)
    {
        return;
    }

    if (NewCameraActor == CurrentFixedCamera && !bIsBlending)
    {
        return;
    }

    UCameraComponent* NewCameraComp = GetCameraComponent(NewCameraActor);
    if (!NewCameraComp)
    {
        return;
    }

    if (CurrentFixedCamera)
    {
        UCameraComponent* CurrentCameraComp = GetCameraComponent(CurrentFixedCamera);
        if (CurrentCameraComp)
        {
            BlendStartLocation = CurrentCameraComp->GetComponentLocation();
            BlendStartRotation = CurrentCameraComp->GetComponentRotation();
            BlendStartFOV = CurrentCameraComp->FieldOfView;
        }
    }
    else
    {
        CurrentFixedCamera = NewCameraActor;
        bIsBlending = false;
        return;
    }

    BlendTargetLocation = NewCameraComp->GetComponentLocation();
    BlendTargetRotation = NewCameraComp->GetComponentRotation();
    BlendTargetFOV = NewCameraComp->FieldOfView;

    TargetFixedCamera = NewCameraActor;
    BlendDuration = BlendTime;
    BlendTimeElapsed = 0.0f;
    BlendAlpha = 0.0f;
    bIsBlending = true;
}

void AFixedCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
    Super::UpdateViewTarget(OutVT, DeltaTime);

    if (bIsBlending && TargetFixedCamera)
    {
        BlendTimeElapsed += DeltaTime;
        BlendAlpha = FMath::Clamp(BlendTimeElapsed / BlendDuration, 0.0f, 1.0f);

        float CurvedAlpha = BlendAlpha * BlendAlpha * (3.0f - 2.0f * BlendAlpha);

        FVector BlendedLocation = FMath::Lerp(BlendStartLocation, BlendTargetLocation, CurvedAlpha);

        FQuat StartQuat = BlendStartRotation.Quaternion();
        FQuat TargetQuat = BlendTargetRotation.Quaternion();
        FQuat BlendedQuat = FQuat::Slerp(StartQuat, TargetQuat, CurvedAlpha);
        FRotator BlendedRotation = BlendedQuat.Rotator();

        float BlendedFOV = FMath::Lerp(BlendStartFOV, BlendTargetFOV, CurvedAlpha);

        OutVT.POV.Location = BlendedLocation;
        OutVT.POV.Rotation = BlendedRotation;
        OutVT.POV.FOV = BlendedFOV;

        if (BlendAlpha >= 1.0f)
        {
            CurrentFixedCamera = TargetFixedCamera;
            TargetFixedCamera = nullptr;
            bIsBlending = false;
        }
    }
    else if (CurrentFixedCamera)
    {
        UCameraComponent* CameraComp = GetCameraComponent(CurrentFixedCamera);
        if (CameraComp)
        {
            OutVT.POV.Location = CameraComp->GetComponentLocation();
            OutVT.POV.Rotation = CameraComp->GetComponentRotation();
            OutVT.POV.FOV = CameraComp->FieldOfView;
        }
    }
}

UCameraComponent* AFixedCameraManager::GetCameraComponent(AActor* Actor) const
{
    if (!Actor)
    {
        return nullptr;
    }

    return Actor->FindComponentByClass<UCameraComponent>();
}

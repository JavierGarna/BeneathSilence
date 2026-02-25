// Fill out your copyright notice in the Description page of Project Settings.


#include "FixedCameraManager.h"
#include "FixedCamera.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

AFixedCameraManager::AFixedCameraManager()
{
    CurrentFixedCamera = nullptr;
    TargetFixedCamera = nullptr;
}

void AFixedCameraManager::SwitchToFixedCamera(AFixedCamera* NewCameraActor, float BlendTime)
{
    if (!NewCameraActor) return;

    if (NewCameraActor == CurrentFixedCamera) return;

    UCameraComponent* NewCameraComp = GetCameraComponent(NewCameraActor);

    if (!NewCameraComp) return;

    if (CurrentFixedCamera)
    {
        UCameraComponent* CurrentCameraComp = GetCameraComponent(CurrentFixedCamera);
    }
    else
    {
        CurrentFixedCamera = NewCameraActor;
        return;
    }

    TargetFixedCamera = NewCameraActor;
}

void AFixedCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
    Super::UpdateViewTarget(OutVT, DeltaTime);

    if (TargetFixedCamera)
    {
        CurrentFixedCamera = TargetFixedCamera;
        TargetFixedCamera = nullptr;
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

UCameraComponent* AFixedCameraManager::GetCameraComponent(AFixedCamera* Actor) const
{
    if (!Actor)
    {
        return nullptr;
    }

    return Actor->FindComponentByClass<UCameraComponent>();
}

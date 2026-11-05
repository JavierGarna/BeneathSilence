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
    if (!IsValid(NewCameraActor)) return;

    UCameraComponent* NewCameraComp = GetCameraComponent(NewCameraActor);

    if (!IsValid(NewCameraComp)) return;

    if (NewCameraActor == CurrentFixedCamera) return;

    CurrentFixedCamera = NewCameraActor;
    TargetFixedCamera = nullptr;
}

void AFixedCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
    Super::UpdateViewTarget(OutVT, DeltaTime);

    if (!IsValid(CurrentFixedCamera)) return;

    UCameraComponent* CameraComp = GetCameraComponent(CurrentFixedCamera);

    if (!IsValid(CameraComp)) return;

    OutVT.POV.Location = CameraComp->GetComponentLocation();
    OutVT.POV.Rotation = CameraComp->GetComponentRotation();
    OutVT.POV.FOV = CameraComp->FieldOfView;
}

UCameraComponent* AFixedCameraManager::GetCameraComponent(AFixedCamera* Actor) const
{
    if (!IsValid(Actor)) return nullptr;

    return Actor->FindComponentByClass<UCameraComponent>();
}

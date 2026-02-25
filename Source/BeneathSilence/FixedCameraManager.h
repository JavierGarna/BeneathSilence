// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "FixedCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class BENEATHSILENCE_API AFixedCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	AFixedCameraManager();

	// Switch to a specific fixed camera
	UFUNCTION(BlueprintCallable, Category = "Fixed Camera")
	void SwitchToFixedCamera(AFixedCamera* NewCameraActor, float BlendTime = 0.5f);

	// Get current active camera
	UFUNCTION(BlueprintPure, Category = "Fixed Camera")
    AFixedCamera* GetCurrentFixedCamera() const { return CurrentFixedCamera; }

protected:
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;

private:
    UPROPERTY()
    AFixedCamera* CurrentFixedCamera;

    UPROPERTY()
    AFixedCamera* TargetFixedCamera;

	UFUNCTION()
    class UCameraComponent* GetCameraComponent(AFixedCamera* Actor) const;
};

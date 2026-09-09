// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class ASoundLight;

UCLASS()
class BENEATHSILENCE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	float GetForwardInputValue() const { return ForwardInputValue; }
	float GetPreviousForwardInputValue() const { return PreviousForwardInputValue; }
	float GetStrafeInputValue() const { return StrafeInputValue; }
	float GetPreviousStrafeInputValue() const { return PreviousStrafeInputValue; }
	float SetPreviousForwardInputValue(float Value) { PreviousForwardInputValue = Value; return PreviousForwardInputValue; }
	float SetPreviousStrafeInputValue(float Value) { PreviousStrafeInputValue = Value; return PreviousStrafeInputValue; }
	void MoveForward(float InputValue, float ReferenceYaw);
	void Strafe(float InputValue, float ReferenceYaw);

	UPROPERTY(EditAnywhere)
	float AgentForwardInput = 0.0f;
	UPROPERTY(EditAnywhere)
	float AgentStrafeInput = 0.0f;

private:
	UPROPERTY(EditAnywhere)
	UInputMappingContext* PlayerMappingContext;
	UPROPERTY(EditAnywhere)
	APlayerController* PlayerController;
	UPROPERTY(EditAnywhere)
	ASoundLight* SoundLightActor;
	UPROPERTY()
	bool bIsRunning = false;
	UPROPERTY(EditAnywhere)
	bool bRunInference = false;

	// Input actions
	UPROPERTY(EditAnywhere)
	UInputAction* MoveForwardsAction;
	UPROPERTY(EditAnywhere)
	UInputAction* StrafeAction;
	UPROPERTY(EditAnywhere)
	UInputAction* RunAction;

	UPROPERTY(EditAnywhere)
	float ForwardInputValue = 0.0f;
	UPROPERTY(EditAnywhere)
	float StrafeInputValue = 0.0f;
	UPROPERTY(EditAnywhere)
	float PreviousForwardInputValue = 0.0f;
	UPROPERTY(EditAnywhere)
	float PreviousStrafeInputValue = 0.0f;

	// Handler declarations
	void MoveForwardsHandler(const FInputActionValue& Value);
	void StrafeHandler(const FInputActionValue& Value);

	UFUNCTION()
	void StartRun();
	UFUNCTION()
	void StopRun();
	UFUNCTION(BlueprintCallable)
	void HandleFootstep(FName InSocketLocation);
};

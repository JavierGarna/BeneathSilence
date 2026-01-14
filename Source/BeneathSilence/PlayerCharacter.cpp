// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "SoundLight.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Possess the player pawn by the lowest numbered player
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Character should not rotate with controller
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Let the CharacterMovementComponent handle rotation
	GetCharacterMovement()->bOrientRotationToMovement = true; // This makes the character face movement direction
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);
	GetCharacterMovement()->GravityScale = 1.0;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;

	// Apply input mapping context to player
	PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		Subsystem->AddMappingContext(PlayerMappingContext, 0);
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRunning)
	{
		GetCharacterMovement()->MaxWalkSpeed = 500.0f; // Running speed
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 300.0f; // Walking speed
	}

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Cast to UEnhancedInputComponent and bind input actions to handler functions
	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	EIC->BindAction(MoveForwardsAction, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveForwardsHandler);
	EIC->BindAction(StrafeAction, ETriggerEvent::Triggered, this, &APlayerCharacter::StrafeHandler);
	EIC->BindAction(RunAction, ETriggerEvent::Started, this, &APlayerCharacter::StartRun);
	EIC->BindAction(RunAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopRun);
}

void APlayerCharacter::MoveForwardsHandler(const FInputActionValue& Value)
{
	const float InputValue = Value.Get<float>();
	if (InputValue == 0.0f)
		return;

	// Get camera forward direction (projected on ground plane)
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && PC->PlayerCameraManager)
	{
		FRotator CameraRotation = PC->PlayerCameraManager->GetCameraRotation();

		// Only use Yaw, ignore pitch/roll to keep movement on ground
		FRotator YawRotation(0.0f, CameraRotation.Yaw, 0.0f);
		FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(ForwardDirection, InputValue);
	}
}

void APlayerCharacter::StrafeHandler(const FInputActionValue& Value)
{
	const float InputValue = Value.Get<float>();
	if (InputValue == 0.0f)
		return;

	// Get camera right direction (projected on ground plane)
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && PC->PlayerCameraManager)
	{
		FRotator CameraRotation = PC->PlayerCameraManager->GetCameraRotation();
		FRotator YawRotation(0.0f, CameraRotation.Yaw, 0.0f);
		FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(RightDirection, InputValue);
	}
}

void APlayerCharacter::StartRun()
{
	bIsRunning = true;
}

void APlayerCharacter::StopRun()
{
	bIsRunning = false;
}

void APlayerCharacter::HandleFootstep(FName InSocketLocation)
{
	if (!SoundLightActor) return;

	if (bIsRunning)
	{
		FVector FootLocation = GetMesh()->GetSocketLocation(InSocketLocation);
		SoundLightActor->StartSoundWave(
			FootLocation,
			1200.f,
			32,
			0.04f
		);
	}
	else
	{
		FVector FootLocation = GetMesh()->GetSocketLocation(InSocketLocation);
		SoundLightActor->StartSoundWave(
			FootLocation,
			800.f,
			32,
			0.06f
		);
	}
}


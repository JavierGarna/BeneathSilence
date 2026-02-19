// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "LearningAgentsManager.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> LearningAgentsManagers;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("LearningAgentsManager"), LearningAgentsManagers);

	for (AActor* Actor : LearningAgentsManagers)
	{
		ULearningAgentsManager* EnemyManager = Actor->FindComponentByClass<ULearningAgentsManager>();

		if (EnemyManager)
		{
			AgentID = EnemyManager->AddAgent(this);
			bManagerFound = true;
			break;
		}
	}

	if (!bManagerFound)
	{
		UE_LOG(LogTemp, Error, TEXT("LearningAgentsManager not found."));
	}
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

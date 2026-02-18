// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyManager.h"
/*
#include "EnemyCharacter.h"
#include "EnemyInteractor.h"
#include "LearningAgentsManager.h"
#include "LearningAgentsPolicy.h"
#include "LearningAgentsCritic.h"
#include "EnemyTrainingEnv.h"
*/

// Sets default values
AEnemyManager::AEnemyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//LearningAgentsManager = CreateDefaultSubobject<ULearningAgentsManager>(TEXT("LearningAgentsManager"));

}

// Called when the game starts or when spawned
void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();

	/*
	// Get EnemyCharacter
	AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(GetWorld()->SpawnActor(AEnemyCharacter::StaticClass()));

	// Get EnemyInteractor
	ULearningAgentsInteractor* EnemyInteractor = ULearningAgentsInteractor::MakeInteractor(LearningAgentsManager, UEnemyInteractor::StaticClass());

	// Get Policy
	ULearningAgentsPolicy* Policy = ULearningAgentsPolicy::MakePolicy(LearningAgentsManager, EnemyInteractor, ULearningAgentsPolicy::StaticClass());

	// Get Critic
	ULearningAgentsCritic* Critic = ULearningAgentsCritic::MakeCritic(LearningAgentsManager, EnemyInteractor, Policy, ULearningAgentsCritic::StaticClass());

	// Get Training Environment
	ULearningAgentsTrainingEnvironment* TrainingEnv = ULearningAgentsTrainingEnvironment::MakeTrainingEnvironment(LearningAgentsManager, UEnemyTrainingEnv::StaticClass());

	// Get Communicator
	//FLearningAgentsTrainerProcessSettings TrainerProcessSettings = FLearningAgentsTrainerProcessSettings();
	//FLearningAgentsSharedMemoryCommunicatorSettings SharedMemorySettings = FLearningAgentsSharedMemoryCommunicatorSettings();
	//FLearningAgentsSharedMemoryTrainerProcess TrainerProcess = ULearningAgentsCommunicatorLibrary::SpawnSharedMemoryTrainingProcess(TrainerProcessSettings, SharedMemorySettings);
	//FLearningAgentsCommunicator Communicator = ULearningAgentsCommunicatorLibrary::MakeSharedMemoryCommunicator(TrainerProcess, TrainerProcessSettings, SharedMemorySettings);

	// Get PPO Trainer
	*/
}

// Called every frame
void AEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyTrainingEnv.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "LearningAgentsRewards.h"
#include "PlayerCharacter.h"

void UEnemyTrainingEnv::GatherAgentReward_Implementation(float& OutReward, const int32 AgentId)
{
	UObject* RewardActor = GetAgent(AgentId, AEnemyCharacter::StaticClass());
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(RewardActor);
	AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(Enemy->GetController());
	APlayerCharacter* Player = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (!Enemy || !EnemyAIController || !Player) return;

	FVector EnemyLocation = Enemy->GetActorLocation();
	FVector PlayerLocation = Player->GetActorLocation();
	float PlayerNoiseLevel = EnemyAIController->GetLearningData().PlayerNoiseLevel;
	FName CurrentState = EnemyAIController->GetLearningData().CurrentState;

	if (CurrentState == "Search")
	{
		OutReward += ULearningAgentsRewards::MakeReward(PlayerNoiseLevel, -10.f);
	}
	else if (CurrentState == "Investigate")
	{
		OutReward += ULearningAgentsRewards::MakeReward(PlayerNoiseLevel, 10.f);
	}
	else if (CurrentState == "Chase")
	{
		OutReward += ULearningAgentsRewards::MakeRewardFromLocationSimilarity(EnemyLocation, PlayerLocation, 200.f, 20.f);
	}
}

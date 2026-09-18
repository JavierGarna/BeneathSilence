// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyTrainingEnv.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "LearningAgentsRewards.h"
#include "PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

void UEnemyTrainingEnv::SetupTrainingEnvironment(ULearningAgentsManager*& InManager)
{
	Super::SetupTrainingEnvironment(InManager);

}

void UEnemyTrainingEnv::GatherAgentReward_Implementation(float& OutReward, const int32 AgentId)
{
	UObject* RewardActor = GetAgent(AgentId, AEnemyCharacter::StaticClass());
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(RewardActor);
	if (!Enemy) return;
	AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(Enemy->GetController());
	if (!EnemyAIController) return;
	APlayerCharacter* Player = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (!Player) return;

	FEnemyLearningData LearningData = EnemyAIController->GetLearningData();

	// Reward based on tension level matching
	float TensionDifference = FMath::Abs(LearningData.CurrentTensionLevel - LearningData.DesiredTensionLevel);
	OutReward += 0.25f * (1.0f - FMath::Clamp(TensionDifference, 0.0f, 1.0f));

	// Player detected -> Chase
	if (LearningData.HasHeardPlayer)
	{
		if (LearningData.CurrentState == Chase)
		{
			OutReward += 0.05f;
		}
	}
	// Recent unknown stimulus -> Investigate
	else if (LearningData.TimeSinceLastStimulus < 5.0f)
	{
		if (LearningData.CurrentState == Investigate)
		{
			const float Recency = 1.0f - (LearningData.TimeSinceLastStimulus / 5.0f);

			OutReward += 0.01f * Recency;
		}
	}
	// Nothing detected for a while -> Search
	else if (LearningData.TimeSinceLastStimulus > 10.0f)
	{
		if (LearningData.CurrentState == Search)
		{
			OutReward += 0.01f;
		}
	}

	ULearningAgentsRewards::MakeReward(OutReward, 1.0f, TEXT("EnemyReward"), true, this, AgentId, Enemy->GetActorLocation(), FLinearColor::Red);
}

void UEnemyTrainingEnv::GatherAgentCompletion_Implementation(ELearningAgentsCompletion& OutCompletion, const int32 AgentId)
{
	UObject* CompletionActor = GetAgent(AgentId, AEnemyCharacter::StaticClass());
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(CompletionActor);
	if (!Enemy) return;
	AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(Enemy->GetController());
	if (!EnemyAIController) return;
	UBlackboardComponent* BlackboardComp = EnemyAIController->GetBlackboardComponent();
	if (!BlackboardComp) return;

	bool bPlayerCaught = BlackboardComp->GetValueAsBool("IsPlayerCaught");
	float TimeSinceLastSeen = BlackboardComp->GetValueAsFloat("TimeSinceLastSeen");

	if (bPlayerCaught) OutCompletion = ULearningAgentsCompletions::MakeCompletionOnCondition(true, ELearningAgentsCompletion::Termination);
	else OutCompletion = ULearningAgentsCompletions::MakeCompletionOnTimeElapsed(TimeSinceLastSeen, 60.f, ELearningAgentsCompletion::Termination);
}

void UEnemyTrainingEnv::ResetAgentEpisodes_Implementation(const TArray<int32>& AgentIds)
{
	UObject* ResetActor = GetAgent(AgentIds[0], AEnemyCharacter::StaticClass());
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(ResetActor);
	if (!Enemy) return;
	AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(Enemy->GetController());
	if (!EnemyAIController) return;
	APlayerCharacter* Player = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (!Player) return;
	
	Enemy->ResetToInitialLocation();
	Player->ResetToInitialLocation();
	EnemyAIController->SetDefaultBlackboardValues();
}

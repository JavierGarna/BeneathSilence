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

	// Setup reward and completion functions
	ULearningAgentsRewards::MakeReward(0.f, 1.f, TEXT("BaseReward"), false, nullptr, -1, FVector::ZeroVector, FLinearColor::Green);
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

	if (LearningData.CurrentDistanceToPlayer <= 200.f)
	{
		OutReward += 50.f;
		return;
	}

	if (LearningData.CurrentState == Search)
	{
		OutReward += 0.25f * LearningData.TimeSinceLastStimulus;
		
		if (LearningData.TimeSinceLastSeen < 5.0f)
		{
			OutReward += 0.25f * LearningData.TimeSinceLastSeen;
		}

		if (LearningData.IsAdjacentToPlayerRoom) 
		{
			OutReward += 0.25f;
		}

		if (LearningData.CurrentDistanceToPlayer < 1000.0f)
		{
			OutReward -= 0.50f;
		}

	}

	if (LearningData.CurrentState == Investigate)
	{
		OutReward += 0.50f * LearningData.LastStimulusStrength;
		OutReward -= 0.25f * LearningData.TimeSinceLastStimulus;

		// Distance to stimulus
		const float DistToStimulus = FVector::Dist(Enemy->GetActorLocation(), LearningData.LastStimulusLocation);
		
		OutReward -= 0.50f * DistToStimulus;
		
	}

	if (LearningData.CurrentState == Chase)
	{
		if (LearningData.CurrentDistanceToPlayer < 1000.0f)
		{
			OutReward += 0.50f;
		}

		if (LearningData.CurrentDistanceToPlayer < 500.0f)
		{
			OutReward += 0.50f;
		}
	}
}

void UEnemyTrainingEnv::GatherAgentCompletion_Implementation(ELearningAgentsCompletion& OutCompletion, const int32 AgentId)
{
	UObject* CompletionActor = GetAgent(AgentId, AEnemyCharacter::StaticClass());
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(CompletionActor);

	bool bPlayerCaught = false;
	float TimeSinceLastStimulus = 0.0f;
	float TimeSinceLastSeen = 0.0f;

	if (Enemy)
	{
		AAIController* AIController = Cast<AAIController>(Enemy->GetController());
		if (AIController)
		{
			UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();

			if (BlackboardComp)
			{
				bPlayerCaught = BlackboardComp->GetValueAsBool("IsPlayerCaught");
				TimeSinceLastSeen = BlackboardComp->GetValueAsFloat("TimeSinceLastSeen");
			}
		}
	}

	OutCompletion = ULearningAgentsCompletions::MakeCompletionOnCondition(bPlayerCaught, ELearningAgentsCompletion::Termination);
	OutCompletion = ULearningAgentsCompletions::MakeCompletionOnTimeElapsed(TimeSinceLastSeen, 60.f, ELearningAgentsCompletion::Termination);

}

void UEnemyTrainingEnv::ResetAgentEpisodes_Implementation(const TArray<int32>& AgentIds)
{
	UObject* ResetActor = GetAgent(AgentIds[0], AEnemyCharacter::StaticClass());
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(ResetActor);

	if (Enemy)
	{
		AAIController* AIController = Cast<AAIController>(Enemy->GetController());
		if (AIController)
		{
			UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
			if (BlackboardComp)
			{
				Enemy->Reset();
				
				//Reset relevant blackboard keys
				BlackboardComp->SetValueAsBool("IsPlayerCaught", false);
				BlackboardComp->SetValueAsFloat("TimeSinceLastSeen", 0.0f);
				BlackboardComp->SetValueAsFloat("TimeSinceLastStimulus", 0.0f);
			}
		}
	}

}

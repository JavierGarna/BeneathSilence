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

	if (LearningData.CurrentDistanceToPlayer <= 200.f)
	{
		OutReward += 50.f;
		return;
	}

	if (LearningData.CurrentState == Search)
	{
		if (LearningData.TimeSinceLastStimulus) OutReward += 0.25f * FMath::Clamp(LearningData.TimeSinceLastStimulus, 0.0f, 20.0f);
		
		if (LearningData.TimeSinceLastSeen < 5.0f)
		{
			OutReward += 0.25f * FMath::Clamp(LearningData.TimeSinceLastSeen, 0.0f, 20.0f);
		}

		if (LearningData.IsAdjacentToPlayerRoom) 
		{
			OutReward += 0.25f;
		}

		if (LearningData.CurrentDistanceToPlayer < 1000.0f)
		{
			OutReward -= 0.25f;
		}

	}
	if (LearningData.CurrentState == Investigate)
	{
		if (LearningData.LastStimulusStrength) OutReward += 0.50f * FMath::Clamp(LearningData.LastStimulusStrength, 0.0f, 1.0f);
		if (LearningData.TimeSinceLastStimulus) OutReward -= 0.25f * FMath::Clamp(LearningData.TimeSinceLastStimulus, 0.0f, 20.0f);

		// Distance to stimulus
		const float DistToStimulus = FVector::Dist(Enemy->GetActorLocation(), LearningData.LastStimulusLocation);
		
		OutReward -= 0.25f * FMath::Clamp(DistToStimulus, 0.0f, 2000.0f);
		
	}

	if (LearningData.CurrentState == Chase)
	{
		if (LearningData.CurrentDistanceToPlayer < 1000.0f)
		{
			OutReward += 0.25f;
		}

		if (LearningData.CurrentDistanceToPlayer < 500.0f)
		{
			OutReward += 0.25f;
		}
	}

	ULearningAgentsRewards::MakeReward(OutReward, 1.0f, TEXT("EnemyReward"), true, this, AgentId, Enemy->GetActorLocation(), FLinearColor::Red);
}

void UEnemyTrainingEnv::GatherAgentCompletion_Implementation(ELearningAgentsCompletion& OutCompletion, const int32 AgentId)
{
	UObject* CompletionActor = GetAgent(AgentId, AEnemyCharacter::StaticClass());
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(CompletionActor);

	bool bPlayerCaught = false;
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

	OutCompletion = ULearningAgentsCompletions::MakeCompletionOnTimeElapsed(TimeSinceLastSeen, 60.f, ELearningAgentsCompletion::Termination);
	if (bPlayerCaught) OutCompletion = ULearningAgentsCompletions::MakeCompletionOnCondition(bPlayerCaught, ELearningAgentsCompletion::Termination);
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

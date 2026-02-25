// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyTrainingEnv.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "LearningAgentsRewards.h"
#include "PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

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

	if (LearningData.CurrentDistanceToPlayer <= CatchDistanceThreshold)
	{
		OutReward += 50.f;
		return;
	}

	OutReward += 0.25f * FMath::Clamp(LearningData.ConfidenceLevel, 0.0f, 1.0f);
	OutReward -= 0.35f * (FMath::Clamp(LearningData.TimeSinceLastSeen, 0.0f, 30.0f) / 30.0f);

	const float StimulusStrength = FMath::Clamp(LearningData.LastStimulusStrength, 0.0f, 1.0f);
	const float StimulusTime = FMath::Clamp(LearningData.TimeSinceLastStimulus, 0.0f, 20.0f);
	const float StimFresh01 = 1.0f - (StimulusTime / 20.0f);

	if (LearningData.CurrentState == Investigate)
	{
		OutReward += 0.50f * StimulusStrength * StimFresh01;
	}
	else
	{
		OutReward -= 0.05f * StimulusStrength * StimFresh01;
	}

	const float Proximity = 1.0f - FMath::Clamp(LearningData.CurrentDistanceToPlayer / MaxUsefulDistance, 0.0f, 1.0f);

	if (LearningData.CurrentState == Chase)
	{
		OutReward += 1.50f * Proximity;

		const float TargetNear = 1.0f - FMath::Clamp(LearningData.CurrentDistanceToPlayer / 2000.0f, 0.0f, 1.0f);
		OutReward += 0.35f * TargetNear;
	}
	else if (LearningData.CurrentState == Search)
	{
		const float DistToLastKnownLocation = FVector::Dist(Enemy->GetActorLocation(), LearningData.LastKnownPlayerLocation);
		const float NearLastKnownLocation = 1.0f - FMath::Clamp(DistToLastKnownLocation / 2500.0f, 0.0f, 1.0f);
		OutReward += 0.25f * NearLastKnownLocation;

		if (Proximity > 0.85f)
		{
			OutReward -= 0.80f;
		}
	}
	else if (LearningData.CurrentState == Investigate)
	{
		const float DistToStimulus = FVector::Dist(Enemy->GetActorLocation(), LearningData.LastStimulusLocation);
		const float NearStimulus = 1.0f - FMath::Clamp(DistToStimulus / 2000.0f, 0.0f, 1.0f);
		OutReward += 0.50f * NearStimulus;

		if (Proximity > 0.85f)
		{
			OutReward -= 0.80f;
		}
	}
	else
	{
		OutReward -= 1.0f;
	}

	const float Desired = FMath::Clamp(LearningData.DesiredTensionLevel, 0.0f, 1.0f);
	const float Current = FMath::Clamp(LearningData.CurrentTensionLevel, 0.0f, 1.0f);
	const float TensionError = FMath::Abs(Desired - Current);
	OutReward += 0.05f * (1.0f - FMath::Clamp(TensionError, 0.0f, 1.0f));

	if (LearningData.TimeSinceLastSeen > 20.0f)
	{
		OutReward -= 0.30f;
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
				TimeSinceLastStimulus = BlackboardComp->GetValueAsFloat("TimeSinceLastStimulus");
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
				BlackboardComp->SetValueAsObject("SelfActor", Enemy);
				BlackboardComp->SetValueAsVector("TargetLocation", FVector::ZeroVector);
				BlackboardComp->SetValueAsInt("TargetRoom", 0);
				BlackboardComp->SetValueAsVector("LastKnownPlayerLocation", FVector::ZeroVector);
				BlackboardComp->SetValueAsInt("LastKnownPlayerRoom", 0);
				BlackboardComp->SetValueAsFloat("TimeSinceLastSeen", 0.0f);
				BlackboardComp->SetValueAsFloat("ConfidenceLevel", 0.0f);
				BlackboardComp->SetValueAsFloat("LastStimulusStrength", 0.0f);
				BlackboardComp->SetValueAsVector("LastStimulusLocation", FVector::ZeroVector);
				BlackboardComp->SetValueAsFloat("TimeSinceLastStimulus", 0.0f);
				BlackboardComp->SetValueAsFloat("CurrentDistanceToPlayer", 0.0f);
				BlackboardComp->SetValueAsBool("IsAdjacentToPlayerRoom", false);
				Enemy->Reset();
			}
		}
	}

}

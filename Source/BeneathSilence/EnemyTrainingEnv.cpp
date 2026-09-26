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
	TensionDifference = FMath::Clamp(TensionDifference, 0.0f, 1.0f);
	const float TensionReward = FMath::Lerp(0.1f, -0.1f, TensionDifference);
	OutReward += TensionReward;

	// Player detected -> Chase
	const float ChaseRecencyScore = 1.0f - FMath::Clamp(LearningData.TimeSinceLastSeen / 10.0f, 0.0f, 1.0f);

	const float ChaseScore = LearningData.HasHeardPlayer ? 1.0f : ChaseRecencyScore;

	// Recent unknown stimulus -> Investigate
	float InvestigateScore = 0.0f;

	if (!LearningData.HasHeardPlayer)
	{
		InvestigateScore = 1.0f - FMath::Clamp(LearningData.TimeSinceLastStimulus / 10.0f, 0.0f, 1.0f);
	}

	// Nothing detected for a while -> Search
	const float TimeScore = FMath::Clamp(LearningData.TimeSinceLastSeen / 10.0f, 0.0f, 1.0f) * FMath::Clamp(LearningData.TimeSinceLastStimulus / 10.0f, 0.0f, 1.0f);
	const float TensionModifier = FMath::Lerp(0.7f, 1.0f, FMath::Clamp(LearningData.DesiredTensionLevel, 0.0f, 1.0f));
	const float SearchScore = TimeScore * TensionModifier;

	// Low tension level desired -> RunAway
	const float TensionScore = 1.0f - FMath::Clamp(LearningData.DesiredTensionLevel, 0.0f, 1.0f);
	const float ProximityScore = 1.0f - FMath::Clamp(LearningData.CurrentDistanceToPlayer / 1200.0f, 0.0f, 1.0f);
	const float RunAwayScore = TensionScore * ProximityScore;

	float StateScore = 0.5f;

	switch (LearningData.CurrentState)
	{
		case Chase:
			StateScore = FMath::Lerp(-1.0f, 1.0f, ChaseScore);
			break;

		case Investigate:
			StateScore = FMath::Lerp(-1.0f, 1.0f, InvestigateScore);
			break;

		case Search:
			StateScore = FMath::Lerp(-1.0f, 1.0f, SearchScore);
			break;

		case RunAway:
			StateScore = FMath::Lerp(-1.0f, 1.0f, RunAwayScore);
			break;
	}

	const float StateReward = StateScore * 0.1f;
	OutReward += StateReward;

	UE_LOG(LogTemp, Warning,
		TEXT("REWARD: CurrentState=%d | Chase=%.2f Investigate=%.2f Search=%.2f RunAway=%.2f | StateScore=%.2f | DesiredTension=%.2f CurrentTension=%.2f"),
		static_cast<int32>(LearningData.CurrentState),
		ChaseScore,
		InvestigateScore,
		SearchScore,
		RunAwayScore,
		StateScore,
		LearningData.DesiredTensionLevel,
		LearningData.CurrentTensionLevel);

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

	// Log TimeSinceLastSeen for debugging
	//UE_LOG(LogTemp, Warning, TEXT("TimeSinceLastSeen: %f"), TimeSinceLastSeen);

	if (bPlayerCaught)
	{
		OutCompletion = ELearningAgentsCompletion::Termination;
		// Log that the player was caught
		UE_LOG(LogTemp, Warning, TEXT("Player Caught!"));
	}
	else if (TimeSinceLastSeen >= 120.0f)
	{
		OutCompletion = ELearningAgentsCompletion::Termination;
		// Log that the player was not seen for too long
		UE_LOG(LogTemp, Warning, TEXT("Player not seen for too long!"));
	}
	else
	{
		OutCompletion = ELearningAgentsCompletion::Running;
	}
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

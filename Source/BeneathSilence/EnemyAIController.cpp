// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TargetPoint.h"
#include "RoomVolume.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AIPerceptionSystem.h"

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (EnemyBehaviorTree)
	{
		RunBehaviorTree(EnemyBehaviorTree);
		BlackboardComp = GetBlackboardComponent();
	}

	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject("SelfActor", GetPawn());
		BlackboardComp->SetValueAsObject("PlayerActor", UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		BlackboardComp->SetValueAsEnum("CurrentState", 0);
		BlackboardComp->SetValueAsEnum("CurrentStrategy", 0);
	}

	UAIPerceptionComponent* PerceptionComp = GetPerceptionComponent();

	if (PerceptionComp) PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsFloat("TimeSinceLastSeen", BlackboardComp->GetValueAsFloat("TimeSinceLastSeen") + DeltaTime);
		BlackboardComp->SetValueAsFloat("TimeSinceLastStimulus", BlackboardComp->GetValueAsFloat("TimeSinceLastStimulus") + DeltaTime);

		if (Player && GetPawn())
		{
			BlackboardComp->SetValueAsFloat("CurrentDistanceToPlayer",FVector::Dist(GetPawn()->GetActorLocation(), Player->GetActorLocation()));
		}
		
		if (PlayerCurrentRoom)
		{
			if (PlayerCurrentRoom->ConnectedRooms.Contains(EnemyCurrentRoom)) BlackboardComp->SetValueAsBool("IsAdjacentToPlayerRoom", true);
			else BlackboardComp->SetValueAsBool("IsAdjacentToPlayerRoom", false);
		}

		if (EnemyCurrentRoom) EnemyCurrentRoom->EnemyTimeInRoom += DeltaTime;
	}
}

FEnemyLearningData AEnemyAIController::GetLearningData()
{
	FEnemyLearningData LearningData;

	LearningData.SelfActor = Cast<AEnemyCharacter>(GetBlackboardComponent()->GetValueAsObject("SelfActor"));
	LearningData.CurrentState = GetBlackboardComponent()->GetValueAsEnum("CurrentState");
	LearningData.CurrentStrategy = GetBlackboardComponent()->GetValueAsEnum("CurrentStrategy");

	LearningData.TargetActor = GetBlackboardComponent()->GetValueAsObject("TargetActor");
	LearningData.TargetLocation = GetBlackboardComponent()->GetValueAsVector("TargetLocation");
	LearningData.TargetRoom = Cast<ARoomVolume>(GetBlackboardComponent()->GetValueAsObject("TargetRoom"));

	LearningData.LastKnownPlayerLocation = GetBlackboardComponent()->GetValueAsVector("LastKnownPlayerLocation");
	LearningData.LastKnownPlayerRoom = GetBlackboardComponent()->GetValueAsInt("LastKnownPlayerRoom");
	LearningData.TimeSinceLastSeen = GetBlackboardComponent()->GetValueAsFloat("TimeSinceLastSeen");
	LearningData.ConfidenceLevel = GetBlackboardComponent()->GetValueAsFloat("ConfidenceLevel");

	LearningData.LastStimulusStrength = GetBlackboardComponent()->GetValueAsFloat("LastStimulusStrength");
	LearningData.LastStimulusLocation = GetBlackboardComponent()->GetValueAsVector("LastStimulusLocation");
	LearningData.TimeSinceLastStimulus = GetBlackboardComponent()->GetValueAsFloat("TimeSinceLastStimulus");

	LearningData.CurrentDistanceToPlayer = GetBlackboardComponent()->GetValueAsFloat("CurrentDistanceToPlayer");
	LearningData.IsAdjacentToPlayerRoom = GetBlackboardComponent()->GetValueAsBool("IsAdjacentToPlayerRoom");

	LearningData.DesiredTensionLevel = GetBlackboardComponent()->GetValueAsFloat("DesiredTensionLevel");
	LearningData.CurrentTensionLevel = GetBlackboardComponent()->GetValueAsFloat("CurrentTensionLevel");

	return LearningData;
}

TArray<float> AEnemyAIController::GetEnemyTimeInRooms()
{
	return EnemyTimeInRooms;
}

TArray<float> AEnemyAIController::GetPlayerTimeInRooms()
{
	return PlayerTimeInRooms;
}

ARoomVolume* AEnemyAIController::GetPlayerCurrentRoom()
{
	return PlayerCurrentRoom;
}

void AEnemyAIController::SetCurrentState(FName NewState)
{
	GetBlackboardComponent()->SetValueAsName("CurrentState", NewState);
}

void AEnemyAIController::SetEnemyCurrentRoom(ARoomVolume* NewRoom)
{
	EnemyCurrentRoom = NewRoom;
}

void AEnemyAIController::SetPlayerCurrentRoom(ARoomVolume* NewRoom)
{
	PlayerCurrentRoom = NewRoom;
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	TSubclassOf<UAISense> SenseClass = UAIPerceptionSystem::GetSenseClassForStimulus(this, Stimulus);

	if (SenseClass == UAISense_Hearing::StaticClass())
	{
		BlackboardComp->SetValueAsVector("LastStimulusLocation", Stimulus.StimulusLocation);
		BlackboardComp->SetValueAsFloat("LastStimulusStrength", Stimulus.Strength);
		BlackboardComp->SetValueAsFloat("TimeSinceLastStimulus", 0.0f);

		// log all stimulus
		UE_LOG(LogTemp, Log, TEXT("Heard stimulus from %s at location %s with strength %f"), *Actor->GetName(), *Stimulus.StimulusLocation.ToString(), Stimulus.Strength);
	}
}

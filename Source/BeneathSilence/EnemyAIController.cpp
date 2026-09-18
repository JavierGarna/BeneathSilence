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

	SetDefaultBlackboardValues();

	UAIPerceptionComponent* PerceptionComp = GetPerceptionComponent();

	if (PerceptionComp) PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
	else UE_LOG(LogTemp, Warning, TEXT("PerceptionComp is null in AEnemyAIController::BeginPlay()"));
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

			if (BlackboardComp->GetValueAsFloat("CurrentDistanceToPlayer") < 500.f)
			{
				BlackboardComp->SetValueAsFloat("CurrentTensionLevel", FMath::Clamp(BlackboardComp->GetValueAsFloat("CurrentTensionLevel") + DeltaTime * 0.2f, 0.f, 1.f));
			}
			
			if (BlackboardComp->GetValueAsFloat("CurrentDistanceToPlayer") > 2000.f)
			{
				BlackboardComp->SetValueAsFloat("CurrentTensionLevel", FMath::Clamp(BlackboardComp->GetValueAsFloat("CurrentTensionLevel") - DeltaTime * 0.2f, 0.f, 1.f));
			}
		}
		
		if (PlayerCurrentRoom) BlackboardComp->SetValueAsObject("PlayerCurrentRoom", PlayerCurrentRoom);
		if (EnemyCurrentRoom) BlackboardComp->SetValueAsObject("EnemyCurrentRoom", EnemyCurrentRoom);

		if (BlackboardComp->GetValueAsBool("IsAdjacentToPlayerRoom"))
		{
			BlackboardComp->SetValueAsFloat("CurrentTensionLevel", FMath::Clamp(BlackboardComp->GetValueAsFloat("CurrentTensionLevel") + DeltaTime * 0.1f, 0.f, 1.f));
		}
		else
		{
			BlackboardComp->SetValueAsFloat("CurrentTensionLevel", FMath::Clamp(BlackboardComp->GetValueAsFloat("CurrentTensionLevel") - DeltaTime * 0.1f, 0.f, 1.f));
		}

		if (BlackboardComp->GetValueAsBool("HasHeardPlayer"))
		{
			HasHeardPlayerTimer += DeltaTime;
			if (HasHeardPlayerTimer >= FMath::FRandRange(5.f, 10.f))
			{
				BlackboardComp->SetValueAsBool("HasHeardPlayer", false);
				HasHeardPlayerTimer = 0;
			}
		}

		if (BlackboardComp->GetValueAsFloat("TimeSinceLastSeen") > 15.f)
		{
			BlackboardComp->SetValueAsFloat("DesiredTensionLevel", FMath::Clamp(BlackboardComp->GetValueAsFloat("DesiredTensionLevel") + DeltaTime * 0.1f, 0.f, 1.f));
		}

		if (BlackboardComp->GetValueAsFloat("CurrentTensionLevel") >= 0.8f)
		{
			BlackboardComp->SetValueAsFloat("DesiredTensionLevel", FMath::Clamp(BlackboardComp->GetValueAsFloat("DesiredTensionLevel") - DeltaTime * 0.05f, 0.f, 1.f));
		}
		else if (BlackboardComp->GetValueAsFloat("CurrentTensionLevel") <= 0.2)
		{
			BlackboardComp->SetValueAsFloat("DesiredTensionLevel", FMath::Clamp(BlackboardComp->GetValueAsFloat("DesiredTensionLevel") + DeltaTime * 0.05f, 0.f, 1.f));
		}
	}

	// Log all data from the blackboard
	UE_LOG(LogTemp, Warning, TEXT("CurrentState: %d"), BlackboardComp->GetValueAsEnum("CurrentState"));
	UE_LOG(LogTemp, Warning, TEXT("CurrentStrategy: %d"), BlackboardComp->GetValueAsEnum("CurrentStrategy"));
	UE_LOG(LogTemp, Warning, TEXT("LastKnownPlayerLocation: %s"), *BlackboardComp->GetValueAsVector("LastKnownPlayerLocation").ToString());
	UE_LOG(LogTemp, Warning, TEXT("CurrentDistanceToPlayer: %f"), BlackboardComp->GetValueAsFloat("CurrentDistanceToPlayer"));
	UE_LOG(LogTemp, Warning, TEXT("IsAdjacentToPlayerRoom: %s"), BlackboardComp->GetValueAsBool("IsAdjacentToPlayerRoom") ? TEXT("Is Adjacent") : TEXT("Is Not Adjacent"));
	UE_LOG(LogTemp, Warning, TEXT("TimeSinceLastSeen: %f"), BlackboardComp->GetValueAsFloat("TimeSinceLastSeen"));
	UE_LOG(LogTemp, Warning, TEXT("LastStimulusStrength: %f"), BlackboardComp->GetValueAsFloat("LastStimulusStrength"));
	UE_LOG(LogTemp, Warning, TEXT("LastStimulusLocation: %s"), *BlackboardComp->GetValueAsVector("LastStimulusLocation").ToString());
	UE_LOG(LogTemp, Warning, TEXT("TimeSinceLastStimulus: %f"), BlackboardComp->GetValueAsFloat("TimeSinceLastStimulus"));
	UE_LOG(LogTemp, Warning, TEXT("HasHeardPlayer: %s"), BlackboardComp->GetValueAsBool("HasHeardPlayer") ? TEXT("Has Heard Player") : TEXT("Has Not Heard Player"));
	UE_LOG(LogTemp, Warning, TEXT("DesiredTensionLevel: %f"), BlackboardComp->GetValueAsFloat("DesiredTensionLevel"));
	UE_LOG(LogTemp, Warning, TEXT("CurrentTensionLevel: %f"), BlackboardComp->GetValueAsFloat("CurrentTensionLevel"));
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
	LearningData.ConfidenceLevel = GetBlackboardComponent()->GetValueAsFloat("StressLevel");

	LearningData.LastStimulusStrength = GetBlackboardComponent()->GetValueAsFloat("LastStimulusStrength");
	LearningData.LastStimulusLocation = GetBlackboardComponent()->GetValueAsVector("LastStimulusLocation");
	LearningData.TimeSinceLastStimulus = GetBlackboardComponent()->GetValueAsFloat("TimeSinceLastStimulus");
	LearningData.HasHeardPlayer = GetBlackboardComponent()->GetValueAsBool("HasHeardPlayer");

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

void AEnemyAIController::SetDefaultBlackboardValues()
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject("SelfActor", GetPawn());
		BlackboardComp->SetValueAsObject("PlayerActor", UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		// State / Strategy
		BlackboardComp->SetValueAsEnum("CurrentState", 0);
		BlackboardComp->SetValueAsEnum("CurrentStrategy", 0);
		// Current target
		BlackboardComp->ClearValue("TargetActor");
		BlackboardComp->ClearValue("TargetLocation");
		BlackboardComp->ClearValue("TargetRoom");
		// Last known player information
		BlackboardComp->ClearValue("LastKnownPlayerLocation");
		BlackboardComp->SetValueAsInt("LastKnownPlayerRoom", -1);
		BlackboardComp->SetValueAsFloat("TimeSinceLastSeen", 0.0f);
		// Confidence / Stress
		BlackboardComp->SetValueAsFloat("StressLevel", 0.0f);
		// Stimulus information
		BlackboardComp->SetValueAsFloat("LastStimulusStrength", 0.0f);
		BlackboardComp->ClearValue("LastStimulusLocation");
		BlackboardComp->SetValueAsFloat("TimeSinceLastStimulus", 0.0f);
		BlackboardComp->SetValueAsBool("HasHeardPlayer", false);
		// Player relationship
		BlackboardComp->SetValueAsFloat("CurrentDistanceToPlayer", 0.0f);
		BlackboardComp->SetValueAsBool("IsAdjacentToPlayerRoom", false);
		// Tension
		BlackboardComp->SetValueAsFloat("CurrentTensionLevel", 0.0f);
		// Completion
		BlackboardComp->SetValueAsBool("IsPlayerCaught", false);

		TArray<AActor*> FoundRooms;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARoomVolume::StaticClass(), FoundRooms);

		for (AActor* Room : FoundRooms)
		{
			AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(GetPawn());
			APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

			if (Room->IsOverlappingActor(EnemyCharacter))
			{
				BlackboardComp->SetValueAsObject(FName("EnemyCurrentRoom"), Room);
			}
			else UE_LOG(LogTemp, Warning, TEXT("EnemyCharacter is not overlapping with any room."));

			if (Room->IsOverlappingActor(PlayerCharacter))
			{
				BlackboardComp->SetValueAsObject(FName("PlayerCurrentRoom"), Room);
			}
			else UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter is not overlapping with any room."));
		}
	}
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	TSubclassOf<UAISense> SenseClass = UAIPerceptionSystem::GetSenseClassForStimulus(this, Stimulus);

	if (SenseClass == UAISense_Hearing::StaticClass())
	{
		BlackboardComp->SetValueAsVector("LastStimulusLocation", Stimulus.StimulusLocation);
		BlackboardComp->SetValueAsFloat("LastStimulusStrength", Stimulus.Strength);
		BlackboardComp->SetValueAsFloat("TimeSinceLastStimulus", 0.0f);
	}
}

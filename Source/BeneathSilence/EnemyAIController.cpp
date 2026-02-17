// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
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

	if (EnemyBehaviorTree)
	{
		RunBehaviorTree(EnemyBehaviorTree);
	}

	TArray<AActor*> RoomActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARoomVolume::StaticClass(), RoomActors);

	UAIPerceptionComponent* PerceptionComp = GetPerceptionComponent();

	if (PerceptionComp)
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
	}

	Rooms.Empty();
	for (AActor* Actor : RoomActors)
	{
		ARoomVolume* Room = Cast<ARoomVolume>(Actor);
		if (Room) Rooms.Add(Room);

	}

}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FEnemyLearningData AEnemyAIController::GetLearningData()
{
	FEnemyLearningData LearningData;

	LearningData.PlayerNoiseLevel = GetBlackboardComponent()->GetValueAsFloat("StimulusStrength");
	LearningData.PlayerPosition = GetBlackboardComponent()->GetValueAsVector("StimulusLocation");
	LearningData.bPlayerVisible = GetBlackboardComponent()->GetValueAsBool("PlayerVisible");
	FName CurrentState = GetBlackboardComponent()->GetValueAsName("CurrentState");

	return LearningData;
}

void AEnemyAIController::SetCurrentState(FName NewState)
{
	GetBlackboardComponent()->SetValueAsName("CurrentState", NewState);
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	TSubclassOf<UAISense> SenseClass = UAIPerceptionSystem::GetSenseClassForStimulus(this, Stimulus);

	if (SenseClass == UAISense_Hearing::StaticClass())
	{
		UBlackboardComponent* BlackboardComp = GetBlackboardComponent();

		BlackboardComp->SetValueAsVector("StimulusLocation", Stimulus.StimulusLocation);
		BlackboardComp->SetValueAsFloat("StimulusStrength", Stimulus.Strength);
	}
}
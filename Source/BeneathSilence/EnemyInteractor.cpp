// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyInteractor.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "RoomVolume.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

void UEnemyInteractor::SpecifyAgentObservation_Implementation(FLearningAgentsObservationSchemaElement& OutObservationSchemaElement, ULearningAgentsObservationSchema* InObservationSchema)
{
	TArray<AActor*> RoomActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARoomVolume::StaticClass(), RoomActors);

	Rooms.Empty();
	for (AActor* Actor : RoomActors)
	{
		ARoomVolume* Room = Cast<ARoomVolume>(Actor);
		if (Room) Rooms.Add(Room);
	}

	// Use the LearningAgents API to specify a struct observation for EnemyData
	TMap<FName, FLearningAgentsObservationSchemaElement> Elements;

	// Sub-elements of the EnemyData struct observation
	Elements.Add(TEXT("CurrentState"), ULearningAgentsObservations::SpecifyExclusiveDiscreteObservation(InObservationSchema, 3, TEXT("CurrentStateObservation")));
	Elements.Add(TEXT("CurrentStrategy"), ULearningAgentsObservations::SpecifyExclusiveDiscreteObservation(InObservationSchema, 3, TEXT("CurrentStrategyObservation")));
	
	Elements.Add(TEXT("LastKnownPlayerLocation"), ULearningAgentsObservations::SpecifyLocationObservation(InObservationSchema, 1.0f, TEXT("LastKnownPlayerLocationObservation")));
	Elements.Add(TEXT("LastKnownPlayerRoom"), ULearningAgentsObservations::SpecifyExclusiveDiscreteObservation(InObservationSchema, Rooms.Num(), TEXT("LastKnownPlayerRoomObservation")));
	Elements.Add(TEXT("TimeSincePlayerSeen"), ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.0f, TEXT("TimeSincePlayerSeenObservation")));
	Elements.Add(TEXT("ConfidenceLevel"), ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.0f, TEXT("ConfidenceLevelObservation")));

	Elements.Add(TEXT("LastStimulusStrength"), ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.0f, TEXT("LastStimulusStrengthObservation")));
	Elements.Add(TEXT("LastStimulusLocation"), ULearningAgentsObservations::SpecifyLocationObservation(InObservationSchema, 1.0f, TEXT("LastStimulusLocationObservation")));
	Elements.Add(TEXT("TimeSinceLastStimulus"), ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.0f, TEXT("TimeSinceLastStimulusObservation")));

	Elements.Add(TEXT("CurrentDistanceToPlayer"), ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.0f, TEXT("CurrentDistanceToPlayerObservation")));
	Elements.Add(TEXT("IsAdjacentToPlayerRoom"), ULearningAgentsObservations::SpecifyBoolObservation(InObservationSchema, TEXT("IsAdjacentToPlayerRoomObservation")));

	OutObservationSchemaElement = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema, Elements, TEXT("Observations"));
}

void UEnemyInteractor::GatherAgentObservation_Implementation(FLearningAgentsObservationObjectElement& OutObservationObjectElement, ULearningAgentsObservationObject* InObservationObject, const int32 AgentId)
{
	UObject* ObsActor = GetAgent(AgentId, AEnemyCharacter::StaticClass());
	TMap<FName, FLearningAgentsObservationObjectElement> Elements;

	FEnemyLearningData LearningData;

	if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(ObsActor))
	{
		if (AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(Enemy->GetController()))
		{
			LearningData = EnemyAIController->GetLearningData();
		}

		Elements.Add(TEXT("CurrentState"), ULearningAgentsObservations::MakeExclusiveDiscreteObservation(InObservationObject, LearningData.CurrentState, TEXT("CurrentStateObservation")));
		Elements.Add(TEXT("CurrentStrategy"), ULearningAgentsObservations::MakeExclusiveDiscreteObservation(InObservationObject, LearningData.CurrentStrategy, TEXT("CurrentStrategyObservation")));

		Elements.Add(TEXT("LastKnownPlayerLocation"), ULearningAgentsObservations::MakeLocationObservation(InObservationObject, LearningData.LastKnownPlayerLocation, FTransform(), TEXT("LastKnownPlayerLocationObservation")));
		Elements.Add(TEXT("LastKnownPlayerRoom"), ULearningAgentsObservations::MakeExclusiveDiscreteObservation(InObservationObject, LearningData.LastKnownPlayerRoom, TEXT("LastKnownPlayerRoomObservation")));
		Elements.Add(TEXT("TimeSincePlayerSeen"), ULearningAgentsObservations::MakeFloatObservation(InObservationObject, LearningData.TimeSinceLastSeen, TEXT("TimeSincePlayerSeenObservation")));
		Elements.Add(TEXT("ConfidenceLevel"), ULearningAgentsObservations::MakeFloatObservation(InObservationObject, LearningData.ConfidenceLevel, TEXT("ConfidenceLevelObservation")));

		Elements.Add(TEXT("LastStimulusStrength"), ULearningAgentsObservations::MakeFloatObservation(InObservationObject, LearningData.LastStimulusStrength, TEXT("LastStimulusStrengthObservation")));
		Elements.Add(TEXT("LastStimulusLocation"), ULearningAgentsObservations::MakeLocationObservation(InObservationObject, LearningData.LastStimulusLocation, FTransform(), TEXT("LastStimulusLocationObservation")));
		Elements.Add(TEXT("TimeSinceLastStimulus"), ULearningAgentsObservations::MakeFloatObservation(InObservationObject, LearningData.TimeSinceLastStimulus, TEXT("TimeSinceLastStimulusObservation")));

		Elements.Add(TEXT("CurrentDistanceToPlayer"), ULearningAgentsObservations::MakeFloatObservation(InObservationObject, LearningData.CurrentDistanceToPlayer, TEXT("CurrentDistanceToPlayerObservation")));
		Elements.Add(TEXT("IsAdjacentToPlayerRoom"), ULearningAgentsObservations::MakeBoolObservation(InObservationObject, LearningData.IsAdjacentToPlayerRoom , TEXT("IsAdjacentToPlayerRoomObservation")));
	}

	OutObservationObjectElement = ULearningAgentsObservations::MakeStructObservation(InObservationObject, Elements, TEXT("Observations"));
}
 
void UEnemyInteractor::SpecifyAgentAction_Implementation(FLearningAgentsActionSchemaElement& OutActionSchemaElement, ULearningAgentsActionSchema* InActionSchema)
{
	TMap<FName, FLearningAgentsActionSchemaElement> Elements;

	Elements.Add(TEXT("CurrentState"), ULearningAgentsActions::SpecifyExclusiveDiscreteAction(InActionSchema, 3, StatePriorProbabilities, TEXT("CurrentStateAction")));
	Elements.Add(TEXT("CurrentStrategy"), ULearningAgentsActions::SpecifyExclusiveDiscreteAction(InActionSchema, 3, StrategyPriorProbabilities, TEXT("CurrentStrategyAction")));

	OutActionSchemaElement = ULearningAgentsActions::SpecifyStructAction(InActionSchema, Elements);
}

void UEnemyInteractor::PerformAgentAction_Implementation(const ULearningAgentsActionObject* InActionObject, const FLearningAgentsActionObjectElement& InActionObjectElement, const int32 AgentId)
{
	UObject* ActActor = GetAgent(AgentId, AEnemyCharacter::StaticClass());

	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(ActActor);
	if (!Enemy) return;

	AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(Enemy->GetController());
	if (!EnemyAIController) return;

	UBlackboardComponent* BlackboardComp = EnemyAIController->GetBlackboardComponent();
	if (!BlackboardComp) return;

	TMap<FName, FLearningAgentsActionObjectElement> ActionStructElements;
	if (!ULearningAgentsActions::GetStructAction(ActionStructElements, InActionObject, InActionObjectElement))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to get struct action."));
		return;
	}

	auto GetElem = [&ActionStructElements](const FName Name) -> FLearningAgentsActionObjectElement*
	{
		return ActionStructElements.Find(Name);
	};


	FLearningAgentsActionObjectElement* Elem = nullptr;
	int32 StateIndex = 0;
	int32 StrategyIndex = 0;

	Elem = GetElem(TEXT("CurrentState"));
	if (!Elem || !ULearningAgentsActions::GetExclusiveDiscreteAction(StateIndex, InActionObject, *Elem, TEXT("CurrentStateAction")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to read action: CurrentState"));
		return;
	}

	Elem = GetElem(TEXT("CurrentStrategy"));
	if (!Elem || !ULearningAgentsActions::GetExclusiveDiscreteAction(StrategyIndex, InActionObject, *Elem, TEXT("CurrentStrategyAction")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to read action: CurrentStrategy"));
		return;
	}

	BlackboardComp->SetValueAsEnum(TEXT("CurrentState"), static_cast<uint8>(StateIndex));
	BlackboardComp->SetValueAsEnum(TEXT("CurrentStrategy"), static_cast<uint8>(StrategyIndex));

	// Optional debug:
	UE_LOG(LogTemp, Warning, TEXT("Action Applied | State=%d Strategy=%d"),
		StateIndex, StrategyIndex);
}

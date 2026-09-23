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

    // -------------------------------------------------------
    // ROOM INFORMATION
    // -------------------------------------------------------
    Elements.Add(TEXT("LastKnownPlayerRoom"), ULearningAgentsObservations::SpecifyExclusiveDiscreteObservation(InObservationSchema, Rooms.Num(), TEXT("LastKnownPlayerRoomObservation")));

    // -------------------------------------------------------
    // PLAYER INFORMATION
    // -------------------------------------------------------
    Elements.Add(TEXT("TimeSincePlayerSeen"), ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.0f, TEXT("TimeSincePlayerSeenObservation")));
    Elements.Add(TEXT("CurrentDistanceToPlayer"), ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.0f, TEXT("CurrentDistanceToPlayerObservation")));
    Elements.Add(TEXT("IsAdjacentToPlayerRoom"), ULearningAgentsObservations::SpecifyBoolObservation(InObservationSchema, TEXT("IsAdjacentToPlayerRoomObservation")));

    // -------------------------------------------------------
    // HEARING / STIMULUS INFORMATION
    // -------------------------------------------------------
    Elements.Add(TEXT("LastStimulusStrength"), ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.0f, TEXT("LastStimulusStrengthObservation")));
    Elements.Add(TEXT("TimeSinceLastStimulus"), ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.0f, TEXT("TimeSinceLastStimulusObservation")));
    Elements.Add(TEXT("HasHeardPlayer"), ULearningAgentsObservations::SpecifyBoolObservation(InObservationSchema, TEXT("HasHeardPlayerObservation")));

    // -------------------------------------------------------
    // TENSION INFORMATION
    // -------------------------------------------------------
    Elements.Add(TEXT("ConfidenceLevel"), ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.0f, TEXT("ConfidenceLevelObservation")));
    Elements.Add(TEXT("DesiredTensionLevel"), ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.0f, TEXT("DesiredTensionLevelObservation")));

	// Create final struct
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

        // -------------------------------------------------------
        // NORMALIZE CONTINUOUS VALUES
        // -------------------------------------------------------

        // 0 seconds = 0
        // 10+ seconds = 1
        const float NormalizedTimeSinceSeen = FMath::Clamp(LearningData.TimeSinceLastSeen / 10.0f, 0.0f, 1.0f);

        // 0 seconds = 0
        // 10+ seconds = 1
        const float NormalizedTimeSinceStimulus = FMath::Clamp(LearningData.TimeSinceLastStimulus / 10.0f, 0.0f, 1.0f);

        // 0 units = 0
        // 2000+ units = 1
        const float NormalizedDistance = FMath::Clamp(LearningData.CurrentDistanceToPlayer / 2000.0f, 0.0f, 1.0f);
        const float NormalizedDesiredTension = FMath::Clamp(LearningData.DesiredTensionLevel, 0.0f, 1.0f);
        const float NormalizedConfidence = FMath::Clamp(LearningData.ConfidenceLevel, 0.0f, 1.0f);

        // If LastStimulusStrength is already 0-1,
        // this simply ensures it remains within that range.
        const float NormalizedStimulusStrength = FMath::Clamp(LearningData.LastStimulusStrength, 0.0f, 1.0f);

        // -------------------------------------------------------
        // ROOM INFORMATION
        // -------------------------------------------------------
        Elements.Add(TEXT("LastKnownPlayerRoom"), ULearningAgentsObservations::MakeExclusiveDiscreteObservation(InObservationObject, LearningData.LastKnownPlayerRoom, TEXT("LastKnownPlayerRoomObservation")));

        // -------------------------------------------------------
        // PLAYER INFORMATION
        // -------------------------------------------------------
        Elements.Add(TEXT("TimeSincePlayerSeen"), ULearningAgentsObservations::MakeFloatObservation(InObservationObject, NormalizedTimeSinceSeen, TEXT("TimeSincePlayerSeenObservation")));
        Elements.Add(TEXT("CurrentDistanceToPlayer"), ULearningAgentsObservations::MakeFloatObservation(InObservationObject, NormalizedDistance, TEXT("CurrentDistanceToPlayerObservation")));
        Elements.Add(TEXT("IsAdjacentToPlayerRoom"), ULearningAgentsObservations::MakeBoolObservation(InObservationObject, LearningData.IsAdjacentToPlayerRoom, TEXT("IsAdjacentToPlayerRoomObservation")));

        // -------------------------------------------------------
        // HEARING / STIMULUS INFORMATION
        // -------------------------------------------------------
        Elements.Add(TEXT("LastStimulusStrength"), ULearningAgentsObservations::MakeFloatObservation(InObservationObject, NormalizedStimulusStrength, TEXT("LastStimulusStrengthObservation")));
        Elements.Add(TEXT("TimeSinceLastStimulus"), ULearningAgentsObservations::MakeFloatObservation(InObservationObject, NormalizedTimeSinceStimulus, TEXT("TimeSinceLastStimulusObservation")));
        Elements.Add(TEXT("HasHeardPlayer"), ULearningAgentsObservations::MakeBoolObservation(InObservationObject, LearningData.HasHeardPlayer, TEXT("HasHeardPlayerObservation")));

        // -------------------------------------------------------
        // TENSION INFORMATION
        // -------------------------------------------------------
        Elements.Add(TEXT("ConfidenceLevel"), ULearningAgentsObservations::MakeFloatObservation(InObservationObject, NormalizedConfidence, TEXT("ConfidenceLevelObservation")));
        Elements.Add(TEXT("DesiredTensionLevel"), ULearningAgentsObservations::MakeFloatObservation(InObservationObject, NormalizedDesiredTension, TEXT("DesiredTensionLevelObservation")));
	}

	// Create final observation
	OutObservationObjectElement = ULearningAgentsObservations::MakeStructObservation(InObservationObject, Elements, TEXT("Observations"));
}
 
void UEnemyInteractor::SpecifyAgentAction_Implementation(FLearningAgentsActionSchemaElement& OutActionSchemaElement, ULearningAgentsActionSchema* InActionSchema)
{
	TMap<FName, FLearningAgentsActionSchemaElement> Elements;

	Elements.Add(TEXT("CurrentState"), ULearningAgentsActions::SpecifyExclusiveDiscreteAction(InActionSchema, 4, {}, TEXT("CurrentStateAction")));
	//Elements.Add(TEXT("CurrentStrategy"), ULearningAgentsActions::SpecifyExclusiveDiscreteAction(InActionSchema, 3, StrategyPriorProbabilities, TEXT("CurrentStrategyAction")));

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

	ULearningAgentsActions::GetStructAction(ActionStructElements,InActionObject,InActionObjectElement,TEXT("StructAction"));
	int32 SelectedState = 0;

	ULearningAgentsActions::GetExclusiveDiscreteAction(SelectedState, InActionObject, ActionStructElements[TEXT("CurrentState")], TEXT("CurrentStateAction"));

	UE_LOG(LogTemp, Warning, TEXT("SelectedState: %d"), SelectedState);

	BlackboardComp->SetValueAsEnum(TEXT("CurrentState"), static_cast<uint8>(SelectedState));
	//BlackboardComp->SetValueAsEnum(TEXT("CurrentStrategy"), static_cast<uint8>(StrategyIndex));
}

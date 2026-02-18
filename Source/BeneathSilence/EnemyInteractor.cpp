// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyInteractor.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.h"

void UEnemyInteractor::SpecifyAgentObservation_Implementation(FLearningAgentsObservationSchemaElement& OutObservationSchemaElement, ULearningAgentsObservationSchema* InObservationSchema)
{
	// Use the LearningAgents API to specify a struct observation for EnemyData
	TMap<FName, FLearningAgentsObservationSchemaElement> Elements;

	// Sub-elements of the EnemyData struct observation
	Elements.Add(TEXT("PlayerNoiseLevel"), ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.0f, TEXT("PlayerNoiseLevel")));
	Elements.Add(TEXT("PlayerPosition"), ULearningAgentsObservations::SpecifyLocationObservation(InObservationSchema, 100.0f, TEXT("PlayerPosition")));
	Elements.Add(TEXT("CurrentState"), ULearningAgentsObservations::SpecifyExclusiveDiscreteObservation(InObservationSchema, 4, TEXT("CurrentState")));

	OutObservationSchemaElement = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema, Elements);
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

		Elements.Add(TEXT("PlayerNoiseLevel"), ULearningAgentsObservations::MakeFloatObservation(InObservationObject, LearningData.PlayerNoiseLevel));
		Elements.Add(TEXT("PlayerPosition"), ULearningAgentsObservations::MakeLocationObservation(InObservationObject, LearningData.PlayerPosition));
	}

	OutObservationObjectElement = ULearningAgentsObservations::MakeStructObservation(InObservationObject, Elements);
}

void UEnemyInteractor::SpecifyAgentAction_Implementation(FLearningAgentsActionSchemaElement& OutActionSchemaElement, ULearningAgentsActionSchema* InActionSchema)
{
	TMap<FName, FLearningAgentsActionSchemaElement> Elements;

	Elements.Add(TEXT("Search"), ULearningAgentsActions::SpecifyFloatAction(InActionSchema, 1.0f, TEXT("Search")));
	Elements.Add(TEXT("Investigate"), ULearningAgentsActions::SpecifyFloatAction(InActionSchema, 1.0f, TEXT("Investigate")));
	Elements.Add(TEXT("Chase"), ULearningAgentsActions::SpecifyFloatAction(InActionSchema, 1.0f, TEXT("Chase")));

	OutActionSchemaElement = ULearningAgentsActions::SpecifyStructAction(InActionSchema, Elements);
}

void UEnemyInteractor::PerformAgentAction_Implementation(const ULearningAgentsActionObject* InActionObject, const FLearningAgentsActionObjectElement& InActionObjectElement, const int32 AgentId)
{
	UObject* ActActor = GetAgent(AgentId, AEnemyCharacter::StaticClass());

	if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(ActActor))
	{
		if (AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(Enemy->GetController()))
		{
			float SearchValue = 0.f;
			float InvestigateValue = 0.f;
			float ChaseValue = 0.f;

			ULearningAgentsActions::GetFloatAction(SearchValue, InActionObject, InActionObjectElement, TEXT("Search"));
			ULearningAgentsActions::GetFloatAction(InvestigateValue, InActionObject, InActionObjectElement, TEXT("Investigate"));
			ULearningAgentsActions::GetFloatAction(ChaseValue, InActionObject, InActionObjectElement, TEXT("Chase"));

			// Determine the action with the highest value and set the Enemy's state accordingly
			if (SearchValue >= InvestigateValue && SearchValue >= ChaseValue)
			{
				EnemyAIController->SetCurrentState("Search");
			}
			else if (InvestigateValue >= SearchValue && InvestigateValue >= ChaseValue)
			{
				EnemyAIController->SetCurrentState("Investigate");
			}
			else if (ChaseValue >= SearchValue && ChaseValue >= InvestigateValue)
			{
				EnemyAIController->SetCurrentState("Chase");
			}
		}
	}
}

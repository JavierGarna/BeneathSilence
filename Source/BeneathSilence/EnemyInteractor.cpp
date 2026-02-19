// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyInteractor.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.h"

void UEnemyInteractor::SpecifyAgentObservation_Implementation(FLearningAgentsObservationSchemaElement& OutObservationSchemaElement, ULearningAgentsObservationSchema* InObservationSchema)
{
	// Use the LearningAgents API to specify a struct observation for EnemyData
	TMap<FName, FLearningAgentsObservationSchemaElement> Elements;

	// Sub-elements of the EnemyData struct observation
	Elements.Add(TEXT("PlayerNoiseLevel"), ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.0f, TEXT("NoiseObservation")));
	Elements.Add(TEXT("PlayerPosition"), ULearningAgentsObservations::SpecifyLocationObservation(InObservationSchema, 1.0f, TEXT("PlayerPositionObservation")));

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

		Elements.Add(TEXT("PlayerNoiseLevel"), ULearningAgentsObservations::MakeFloatObservation(InObservationObject, LearningData.PlayerNoiseLevel, TEXT("NoiseObservation")));
		Elements.Add(TEXT("PlayerPosition"), ULearningAgentsObservations::MakeLocationObservation(InObservationObject, LearningData.PlayerPosition, FTransform(), TEXT("PlayerPositionObservation")));
	}

	OutObservationObjectElement = ULearningAgentsObservations::MakeStructObservation(InObservationObject, Elements, TEXT("Observations"));
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
			TMap<FName, FLearningAgentsActionObjectElement> ActionStructElements;
			const bool bGotStruct = ULearningAgentsActions::GetStructAction(ActionStructElements, InActionObject, InActionObjectElement);

			if (!bGotStruct)
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to get struct action 'Action'"));
				return;
			}

			float SearchValue = 0.f;
			float InvestigateValue = 0.f;
			float ChaseValue = 0.f;

			const bool bGotSearch = ULearningAgentsActions::GetFloatAction(SearchValue, InActionObject, ActionStructElements[TEXT("Search")], TEXT("Search"));

			const bool bGotInvestigate = ULearningAgentsActions::GetFloatAction(InvestigateValue, InActionObject, ActionStructElements[TEXT("Investigate")], TEXT("Investigate"));

			const bool bGotChase = ULearningAgentsActions::GetFloatAction(ChaseValue, InActionObject, ActionStructElements[TEXT("Chase")], TEXT("Chase"));

			if (!bGotSearch || !bGotInvestigate || !bGotChase)
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to read one or more float actions (Search/Investigate/Chase)"));
				return;
			}


			// Determine the action with the highest value and set the Enemy's state accordingly
			if (SearchValue >= InvestigateValue && SearchValue >= ChaseValue)
			{
				EnemyAIController->SetCurrentState("Search");
				UE_LOG(LogTemp, Display, TEXT("Search chosen"));
			}
			else if (InvestigateValue >= SearchValue && InvestigateValue >= ChaseValue)
			{
				EnemyAIController->SetCurrentState("Investigate");
				UE_LOG(LogTemp, Display, TEXT("Investigate chosen"));
			}
			else if (ChaseValue >= SearchValue && ChaseValue >= InvestigateValue)
			{
				EnemyAIController->SetCurrentState("Chase");
				UE_LOG(LogTemp, Display, TEXT("Chase chosen"));
			}
		}
	}
}

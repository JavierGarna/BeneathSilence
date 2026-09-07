// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInteractor.h"
#include "PlayerCharacter.h"

void UPlayerInteractor::SpecifyAgentObservation_Implementation(FLearningAgentsObservationSchemaElement& OutObservationSchemaElement, ULearningAgentsObservationSchema* InObservationSchema)
{
	// Use the LearningAgents API to specify a struct observation for PlayerData
	TMap<FName, FLearningAgentsObservationSchemaElement> Elements;

	// Sub-elements of the PlayerData struct observation
	Elements.Add(TEXT("ForwardInputValue"), ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 600.0f, TEXT("ForwardInputValueObservation")));
	Elements.Add(TEXT("StrafeInputValue"), ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 600.0f, TEXT("StrafeInputValueObservation")));

	OutObservationSchemaElement = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema, Elements, TEXT("Observations"));
}

void UPlayerInteractor::GatherAgentObservation_Implementation(FLearningAgentsObservationObjectElement& OutObservationObjectElement, ULearningAgentsObservationObject* InObservationObject, const int32 AgentId)
{
	APlayerCharacter* PlayerAI = Cast<APlayerCharacter>(GetAgent(AgentId));

	if (!PlayerAI) return;

	float ForwardInputValue = PlayerAI->GetForwardInputValue();
	float StrafeInputValue = PlayerAI->GetStrafeInputValue();


	// Use the LearningAgents API to gather a struct observation for PlayerData
	TMap<FName, FLearningAgentsObservationObjectElement> Elements;

	// Sub-elements of the PlayerData struct observation
	Elements.Add(TEXT("ForwardInputValue"), ULearningAgentsObservations::MakeFloatObservation(InObservationObject, ForwardInputValue, TEXT("ForwardInputValueObservation")));
	Elements.Add(TEXT("StrafeInputValue"), ULearningAgentsObservations::MakeFloatObservation(InObservationObject, StrafeInputValue, TEXT("StrafeInputValueObservation")));

	OutObservationObjectElement = ULearningAgentsObservations::MakeStructObservation(InObservationObject, Elements, TEXT("Observations"));
}

void UPlayerInteractor::SpecifyAgentAction_Implementation(FLearningAgentsActionSchemaElement& OutActionSchemaElement, ULearningAgentsActionSchema* InActionSchema)
{
	// Use the LearningAgents API to specify a struct action for PlayerData
	TMap<FName, FLearningAgentsActionSchemaElement> Elements;

	// Sub-elements of the PlayerData struct action
	Elements.Add(TEXT("ForwardInputValue"), ULearningAgentsActions::SpecifyFloatAction(InActionSchema, 600.0f, TEXT("ForwardInputValueAction")));
	Elements.Add(TEXT("StrafeInputValue"), ULearningAgentsActions::SpecifyFloatAction(InActionSchema, 600.0f, TEXT("StrafeInputValueAction")));

	OutActionSchemaElement = ULearningAgentsActions::SpecifyStructAction(InActionSchema, Elements, TEXT("Actions"));
}

void UPlayerInteractor::PerformAgentAction_Implementation(const ULearningAgentsActionObject* InActionObject, const FLearningAgentsActionObjectElement& InActionObjectElement, const int32 AgentId)
{
	// Use the LearningAgents API to perform a struct action for PlayerData
	APlayerCharacter* PlayerAI = Cast<APlayerCharacter>(GetAgent(AgentId));

	if (!PlayerAI) return;

	TMap<FName, FLearningAgentsActionObjectElement> Elements;

	const bool bGotStruct =
		ULearningAgentsActions::GetStructAction(
			Elements,
			InActionObject,
			InActionObjectElement,
			TEXT("Actions")
		);

	if (!bGotStruct) return;

	float ForwardValue = 0.0f;
	float StrafeValue = 0.0f;

	ULearningAgentsActions::GetFloatAction(ForwardValue, InActionObject, Elements[TEXT("ForwardInputValue")], TEXT("ForwardInputValueAction"));

	ULearningAgentsActions::GetFloatAction(StrafeValue, InActionObject, Elements[TEXT("StrafeInputValue")], TEXT("StrafeInputValueAction"));
	const float ReferenceYaw = PlayerAI->GetActorRotation().Yaw;

	PlayerAI->MoveForward(ForwardValue, ReferenceYaw);
	PlayerAI->Strafe(StrafeValue, ReferenceYaw);
}
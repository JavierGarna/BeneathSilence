// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAIController.h"
#include "PlayerCharacter.h"

void UPlayerAIController::EvaluateAgentController_Implementation(FLearningAgentsActionObjectElement& OutActionObjectElement, ULearningAgentsActionObject* InActionObject, const ULearningAgentsObservationObject* InObservationObject, const FLearningAgentsObservationObjectElement& InObservationObjectElement, const int32 AgentId)
{
	ControllerActor = GetAgent(AgentId);
	PlayerCharacter = Cast<APlayerCharacter>(ControllerActor);

	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter is null."));
		return;
	}

	float ForwardInput = PlayerCharacter->GetForwardInputValue();
	float StrafeInput = PlayerCharacter->GetStrafeInputValue();

	// Print forward and strafe input values to the log for debugging
	UE_LOG(LogTemp, Log, TEXT("ForwardInput: %f, StrafeInput: %f"), ForwardInput, StrafeInput);
	
	const FLearningAgentsActionObjectElement ForwardAction = ULearningAgentsActions::MakeFloatAction(InActionObject, ForwardInput, TEXT("ForwardInputValueAction"));
	const FLearningAgentsActionObjectElement StrafeAction = ULearningAgentsActions::MakeFloatAction(InActionObject, StrafeInput, TEXT("StrafeInputValueAction"));

	TMap<FName, FLearningAgentsActionObjectElement> Actions;
	Actions.Add(TEXT("ForwardInputValue"), ForwardAction);
	Actions.Add(TEXT("StrafeInputValue"), StrafeAction);

	OutActionObjectElement = ULearningAgentsActions::MakeStructAction(InActionObject, Actions, TEXT("Actions"));
}

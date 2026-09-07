// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsController.h"
#include "PlayerAIController.generated.h"

class APlayerCharacter;

/**
 * 
 */
UCLASS()
class BENEATHSILENCE_API UPlayerAIController : public ULearningAgentsController
{
	GENERATED_BODY()
	
private:
	virtual void EvaluateAgentController_Implementation(
		FLearningAgentsActionObjectElement& OutActionObjectElement,
		ULearningAgentsActionObject* InActionObject,
		const ULearningAgentsObservationObject* InObservationObject,
		const FLearningAgentsObservationObjectElement& InObservationObjectElement,
		const int32 AgentId) override;

	UObject* ControllerActor = nullptr;
	APlayerCharacter* PlayerCharacter = nullptr;

};

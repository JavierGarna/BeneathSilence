// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsInteractor.h"
#include "PlayerInteractor.generated.h"

/**
 * 
 */
UCLASS()
class BENEATHSILENCE_API UPlayerInteractor : public ULearningAgentsInteractor
{
	GENERATED_BODY()

protected:
	virtual void SpecifyAgentObservation_Implementation(FLearningAgentsObservationSchemaElement& OutObservationSchemaElement, ULearningAgentsObservationSchema* InObservationSchema) override;
	virtual void GatherAgentObservation_Implementation(FLearningAgentsObservationObjectElement& OutObservationObjectElement, ULearningAgentsObservationObject* InObservationObject, const int32 AgentId) override;
	virtual void SpecifyAgentAction_Implementation(FLearningAgentsActionSchemaElement& OutActionSchemaElement, ULearningAgentsActionSchema* InActionSchema) override;
	virtual void PerformAgentAction_Implementation(const ULearningAgentsActionObject* InActionObject, const FLearningAgentsActionObjectElement& InActionObjectElement, const int32 AgentId) override;
	
private:
	float QuantizeInput(float value);
	float GetObstacleDistance(AActor* Actor, float AngleDegrees, float TraceLength) const;
	float ForwardInputValue = 0.0f;
	float StrafeInputValue = 0.0f;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsInteractor.h"
#include "EnemyInteractor.generated.h"

class ARoomVolume;

/**
 * 
 */
UCLASS()
class BENEATHSILENCE_API UEnemyInteractor : public ULearningAgentsInteractor
{
	GENERATED_BODY()
	
protected:
	virtual void SpecifyAgentObservation_Implementation(FLearningAgentsObservationSchemaElement& OutObservationSchemaElement, ULearningAgentsObservationSchema* InObservationSchema) override;
	virtual void GatherAgentObservation_Implementation(FLearningAgentsObservationObjectElement& OutObservationObjectElement, ULearningAgentsObservationObject* InObservationObject, const int32 AgentId) override;
	virtual void SpecifyAgentAction_Implementation(FLearningAgentsActionSchemaElement& OutActionSchemaElement, ULearningAgentsActionSchema* InActionSchema) override;
    virtual void PerformAgentAction_Implementation(const ULearningAgentsActionObject* InActionObject, const FLearningAgentsActionObjectElement& InActionObjectElement, const int32 AgentId) override;

private:
	TArray<ARoomVolume*> Rooms;
	TArray<float> StatePriorProbabilities = { 0.25f, 0.25f, 0.50f };
	TArray<float> StrategyPriorProbabilities = { 0.50f, 0.25f, 0.25f };
};
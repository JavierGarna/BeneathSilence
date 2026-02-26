// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsTrainingEnvironment.h"
#include "EnemyTrainingEnv.generated.h"

/**
 * 
 */
UCLASS()
class BENEATHSILENCE_API UEnemyTrainingEnv : public ULearningAgentsTrainingEnvironment
{
	GENERATED_BODY()
	
protected:
	virtual void SetupTrainingEnvironment(ULearningAgentsManager*& InManager);
	virtual void GatherAgentReward_Implementation(float& OutReward, const int32 AgentId) override;
	virtual void GatherAgentCompletion_Implementation(ELearningAgentsCompletion& OutCompletion, const int32 AgentId) override;
	virtual void ResetAgentEpisodes_Implementation(const TArray<int32>& AgentIds) override;

};

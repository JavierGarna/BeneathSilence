// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_Investigate.generated.h"

/**
 * 
 */
UCLASS()
class BENEATHSILENCE_API UBTTask_Investigate : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_Investigate();

protected:
	// Called when the game starts or when spawned
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector StimulusLocationKey;
};

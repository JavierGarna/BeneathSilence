// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_Search.generated.h"

/**
 * 
 */
UCLASS()
class BENEATHSILENCE_API UBTTask_Search : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_Search();
	
protected:
	// Called when the game starts or when spawned
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory) override;
};

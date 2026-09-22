// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_RunAway.generated.h"

class ARoomVolume;
class UNavigationSystemV1;

/**
 * 
 */
UCLASS()
class BENEATHSILENCE_API UBTTask_RunAway : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public: 
	UBTTask_RunAway();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory) override;

private:
	void GetAllRooms();
	bool GetRandomLocationInRoom(ARoomVolume* Room, UNavigationSystemV1* NavSys, FNavLocation& OutLocation);
	TArray<ARoomVolume*> Rooms;
	
};

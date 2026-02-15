// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Chase.h"
#include "PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Chase::UBTTask_Chase()
{
	NodeName = "Chase";
}

EBTNodeResult::Type UBTTask_Chase::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComponent, NodeMemory);

	// Get player location
	APlayerCharacter* Player = GetWorld()->GetFirstPlayerController()->GetPawn<APlayerCharacter>();

	if (Player)
	{
		// Set the target location in the blackboard
		UBlackboardComponent* BlackboardComp = OwnerComponent.GetBlackboardComponent();
		if (BlackboardComp)
		{
			BlackboardComp->SetValueAsObject(GetSelectedBlackboardKey(), Player);
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}

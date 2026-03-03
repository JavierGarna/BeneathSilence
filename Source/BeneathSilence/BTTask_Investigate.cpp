// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Investigate.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Investigate::UBTTask_Investigate()
{
	NodeName = "Investigate";
}

EBTNodeResult::Type UBTTask_Investigate::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComponent, NodeMemory);

	UBlackboardComponent* BlackboardComp = OwnerComponent.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	FVector StimulusLocation = BlackboardComp->GetValueAsVector(StimulusLocationKey.SelectedKeyName);

	if (!StimulusLocation.IsZero())
	{
		BlackboardComp->ClearValue(StimulusLocationKey.SelectedKeyName);
		BlackboardComp->SetValueAsVector(GetSelectedBlackboardKey(), StimulusLocation);

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

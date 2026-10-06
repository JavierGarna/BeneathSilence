// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Investigate.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.h"

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
		AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(OwnerComponent.GetAIOwner());
		AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(EnemyAIController->GetPawn());

		if (Enemy)
		{
			Enemy->bIsRunning = true;
		}

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

void UBTTask_Investigate::OnTaskFinished(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComponent, NodeMemory, TaskResult);

	AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(OwnerComponent.GetAIOwner());
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(EnemyAIController->GetPawn());
	if (Enemy)
	{
		Enemy->bIsRunning = false;
	}
}

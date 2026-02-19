// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_Chase.h"
#include "PlayerCharacter.h"
#include "EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_Chase::UBTTask_Chase()
{
	NodeName = "Chase";
}

EBTNodeResult::Type UBTTask_Chase::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComponent, NodeMemory);

	// Get player location
	APlayerCharacter* Player = GetWorld()->GetFirstPlayerController()->GetPawn<APlayerCharacter>();
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (Player && Enemy)
	{
		// Set the target location in the blackboard
		UBlackboardComponent* BlackboardComp = OwnerComponent.GetBlackboardComponent();
		if (BlackboardComp)
		{
			BlackboardComp->SetValueAsObject(GetSelectedBlackboardKey(), Player);

			if ((Player->GetActorLocation() - Enemy->GetActorLocation()).Size() <= 200.f)
			{
				UGameplayStatics::ApplyDamage(Player, 20.f, Enemy->GetController(), Enemy, UDamageType::StaticClass());

				// Set player caught condition in blackboard to true
				BlackboardComp->SetValueAsBool("PlayerCaught", true);
			}

			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}

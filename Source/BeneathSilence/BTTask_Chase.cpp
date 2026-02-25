// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_Chase.h"
#include "PlayerCharacter.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "RoomVolume.h"

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
		AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(Enemy->GetController());

		UBlackboardComponent* BlackboardComp = OwnerComponent.GetBlackboardComponent();
		if (BlackboardComp)
		{
			BlackboardComp->SetValueAsVector("TargetLocation", Player->GetActorLocation());

			if ((Player->GetActorLocation() - Enemy->GetActorLocation()).Size() <= 50.f)
			{
				UGameplayStatics::ApplyDamage(Player, 20.f, Enemy->GetController(), Enemy, UDamageType::StaticClass());

				BlackboardComp->SetValueAsBool("IsPlayerCaught", true);
				BlackboardComp->SetValueAsVector("LastKnownPlayerLocation", Player->GetActorLocation());
				BlackboardComp->SetValueAsObject("LastKnownPlayerRoom", Cast<UObject>(EnemyAIController->GetPlayerCurrentRoom()));
				BlackboardComp->SetValueAsFloat("TimeSinceLastSeen", 0.f);
			}

			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}

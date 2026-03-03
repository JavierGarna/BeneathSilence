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

	AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(OwnerComponent.GetAIOwner());

	// Get player location
	APlayerCharacter* Player = Cast<APlayerCharacter>(EnemyAIController->GetBlackboardComponent()->GetValueAsObject("PlayerActor"));
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(EnemyAIController->GetPawn());

	if (Player && Enemy)
	{
		UBlackboardComponent* BlackboardComp = OwnerComponent.GetBlackboardComponent();

		if (BlackboardComp)
		{
			// log both locations for debugging
			UE_LOG(LogTemp, Display, TEXT("Player Location: %s"), *Player->GetActorLocation().ToString());
			UE_LOG(LogTemp, Display, TEXT("Enemy Location: %s"), *Enemy->GetActorLocation().ToString());

			if (FVector::Dist(Player->GetActorLocation(), Enemy->GetActorLocation()) <= 100.f)
			{
				UE_LOG(LogTemp, Display, TEXT("Enemy near player"));
				UGameplayStatics::ApplyDamage(Player, 20.f, Enemy->GetController(), Enemy, UDamageType::StaticClass());
				BlackboardComp->SetValueAsBool("IsPlayerCaught", true);
			}

			BlackboardComp->SetValueAsVector("LastKnownPlayerLocation", Player->GetActorLocation());
			BlackboardComp->SetValueAsObject("LastKnownPlayerRoom", Cast<UObject>(EnemyAIController->GetPlayerCurrentRoom()));
			BlackboardComp->SetValueAsFloat("TimeSinceLastSeen", 0.f);

			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}

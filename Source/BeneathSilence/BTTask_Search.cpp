// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Search.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Search::UBTTask_Search()
{
    NodeName = "Search";
}

EBTNodeResult::Type UBTTask_Search::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComponent, NodeMemory);

	// Get the AI Controller
    AAIController* AIController = OwnerComponent.GetAIOwner();
    if (AIController == nullptr)
    {
        return EBTNodeResult::Failed;
    }

	// Get the AI Pawn
    AActor* AIActor = AIController->GetPawn();
    if (AIActor == nullptr)
    {
        return EBTNodeResult::Failed;
    }

	float RandomRadius = 2000.0f; // Define the search radius
	FNavLocation RandomLocation; // To store the random location

    // Get the world from the AI actor
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(AIActor->GetWorld());

    if (NavSys == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    // Get a random reachable point
    bool bSuccess = NavSys->GetRandomReachablePointInRadius(
        AIActor->GetActorLocation(),
        RandomRadius,
        RandomLocation
    );

    if (!bSuccess)
    {
        return EBTNodeResult::Failed;
    }

	// Set the random location in the blackboard
    OwnerComponent.GetBlackboardComponent()->SetValueAsVector(
        GetSelectedBlackboardKey(),
        RandomLocation.Location
    );    

    return EBTNodeResult::Succeeded;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Search.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "RoomVolume.h"
#include "Kismet/GameplayStatics.h"

UBTTask_Search::UBTTask_Search()
{
    NodeName = "Search";
}

EBTNodeResult::Type UBTTask_Search::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComponent, NodeMemory);

    AAIController* AIController = OwnerComponent.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    APawn* AIPawn = AIController->GetPawn();
    if (!AIPawn) return EBTNodeResult::Failed;

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(AIPawn->GetWorld());
    if (!NavSys) return EBTNodeResult::Failed;

    UBlackboardComponent* BlackboardComp = OwnerComponent.GetBlackboardComponent();
    GetAllRooms();

    for (ARoomVolume* Room : Rooms)
    {
        if (Room->RoomName == BlackboardComp->GetValueAsName(RoomKey.SelectedKeyName))
        {
			AssignedRoom = Room;
            break;
        }
    }

	if (!AssignedRoom) return EBTNodeResult::Failed;

    FNavLocation RandomLocation;
    if (GetRandomLocationInRoom(AssignedRoom, NavSys, RandomLocation))
    {
        BlackboardComp->SetValueAsVector(GetSelectedBlackboardKey(), RandomLocation.Location);
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}

void UBTTask_Search::GetAllRooms()
{
    TArray<AActor*> RoomActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARoomVolume::StaticClass(), RoomActors);

    Rooms.Empty();
    for (AActor* Actor : RoomActors)
    {
        ARoomVolume* Room = Cast<ARoomVolume>(Actor);
        if (Room) Rooms.Add(Room);

    }
}

bool UBTTask_Search::GetRandomLocationInRoom(ARoomVolume* Room, UNavigationSystemV1* NavSys, FNavLocation& OutLocation)
{
    if (!Room || !NavSys) return false;

    FVector Origin, BoxExtent;
    Room->GetActorBounds(false, Origin, BoxExtent);
    const float SearchRadius = BoxExtent.Size2D();

    if (NavSys->GetRandomReachablePointInRadius(Origin, SearchRadius, OutLocation))
    {
        // Verify the point is actually inside the room volume
        if (Room->EncompassesPoint(OutLocation.Location))
        {
            return true;
        }
    }

    return false;
}

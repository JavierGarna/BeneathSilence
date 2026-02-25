// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Search.h"
#include "EnemyAIController.h"
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

    BlackboardComp = OwnerComponent.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

    GetAllRooms();

	CurrentStrategy = BlackboardComp->GetValueAsEnum("CurrentStrategy");

    if (CurrentStrategy == Normal)
    {
        for (ARoomVolume* Room : Rooms)
        {
            if (Room->EnemyTimeInRoom <= 10.f)
            {
                TargetRoom = Room;
                break;
            }
        }
    }

    if (CurrentStrategy == Aggressive)
    {
		ARoomVolume* PlayerCurrentRoom = Cast<ARoomVolume>(BlackboardComp->GetValueAsObject("PlayerCurrentRoom"));
		// Adjacent rooms are prioritized in aggressive strategy
        for (ARoomVolume* Room : Rooms)
        {
            if (PlayerCurrentRoom && PlayerCurrentRoom->ConnectedRooms.Contains(Room))
            {
                TargetRoom = Room;
                break;
            }
        }
    }

    if (CurrentStrategy == Cautious)
    {
        ARoomVolume* PlayerCurrentRoom = Cast<ARoomVolume>(BlackboardComp->GetValueAsObject("PlayerCurrentRoom"));
        // Non-adjacent rooms are prioritized in aggressive strategy
        for (ARoomVolume* Room : Rooms)
        {
            if (PlayerCurrentRoom && !PlayerCurrentRoom->ConnectedRooms.Contains(Room))
            {
                TargetRoom = Room;
                break;
            }
        }
	}

	if (!TargetRoom) return EBTNodeResult::Failed;

    FNavLocation RandomLocation;
    if (GetRandomLocationInRoom(TargetRoom, NavSys, RandomLocation))
    {
        BlackboardComp->SetValueAsVector(GetSelectedBlackboardKey(), RandomLocation.Location);
		BlackboardComp->SetValueAsObject("TargetRoom", TargetRoom);
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

    FVector RandomPoint = Origin + FVector(
        FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
        FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
        FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
    );

    if (!Room->EncompassesPoint(RandomPoint)) return false;

    FNavLocation Projected;
    if (NavSys->ProjectPointToNavigation(RandomPoint, Projected, FVector(50.f, 50.f, 150.f)))
    {
        if (Room->EncompassesPoint(Projected.Location))
        {
            OutLocation = Projected;
            return true;
        }
    }

    return false;
}

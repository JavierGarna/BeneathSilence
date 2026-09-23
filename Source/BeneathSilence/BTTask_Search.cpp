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
    if (!AIController)
    {
		UE_LOG(LogTemp, Warning, TEXT("AIController is null in UBTTask_Search::ExecuteTask"));
        return EBTNodeResult::Failed;
    }

    APawn* AIPawn = AIController->GetPawn();
    if (!AIPawn)
    {
		UE_LOG(LogTemp, Warning, TEXT("AIPawn is null in UBTTask_Search::ExecuteTask"));
        return EBTNodeResult::Failed;
    }

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(AIPawn->GetWorld());
    if (!NavSys)
    {
		UE_LOG(LogTemp, Warning, TEXT("Navigation System is null in UBTTask_Search::ExecuteTask"));
        return EBTNodeResult::Failed;
    }

    BlackboardComp = OwnerComponent.GetBlackboardComponent();
    if (!BlackboardComp)
    {
		UE_LOG(LogTemp, Warning, TEXT("BlackboardComp is null in UBTTask_Search::ExecuteTask"));
        return EBTNodeResult::Failed;
    }

    GetAllRooms();

	CurrentStrategy = BlackboardComp->GetValueAsEnum("CurrentStrategy");
	CurrentRoom = Cast<ARoomVolume>(BlackboardComp->GetValueAsObject("EnemyCurrentRoom"));
  
    if (CurrentStrategy == Normal)
    {
		bool bFoundCurrentRoom = false;

        if (CurrentRoom->EnemyTimeInRoom >= 10.f)
        {
			for (ARoomVolume* AdjacentRoom : CurrentRoom->ConnectedRooms)
			{
				if (!IsValid(AdjacentRoom)) continue;
                if (AdjacentRoom->bIsLocked) continue;

				if (AdjacentRoom->EnemyTimeInRoom < 10.0f)
				{
					CurrentRoom = AdjacentRoom;
					bFoundCurrentRoom = true;
					break;
				}
			}

            while (!bFoundCurrentRoom)
            {
                for (ARoomVolume* Room : Rooms)
                {
                    if (!IsValid(Room)) continue;
					if (Room->bIsLocked) continue;

                    if (Room->EnemyTimeInRoom < 10.0f)
                    {
                        CurrentRoom = Room;
                        bFoundCurrentRoom = true;
                        break;
                    }
                }

				if (!bFoundCurrentRoom)
				{
					for (ARoomVolume* Room : Rooms)
					{
						Room->EnemyTimeInRoom = 0.0f; // Reset enemy time for all rooms
					}
				}
            }
        }
    }
   
    /*
	if (CurrentStrategy == Aggressive) // Prioritise adjacent rooms with no recent enemy activity in aggressive strategy
    {
		ARoomVolume* PlayerCurrentRoom = Cast<ARoomVolume>(BlackboardComp->GetValueAsObject("PlayerCurrentRoom"));

        if (PlayerCurrentRoom)
        {
            for (ARoomVolume* Room : PlayerCurrentRoom->ConnectedRooms)
            {
                if (!IsValid(Room)) continue;

                if (Room->EnemyTimeInRoom < 10.0f)
                {
                    TargetRoom = Room;
                    break; // found a high-priority room
                }
            }

			if (!TargetRoom)
            {
                for (ARoomVolume* Room : PlayerCurrentRoom->ConnectedRooms)
                {
                    if (!IsValid(Room)) continue;

                    TargetRoom = Room;
                    break;
                }
            }

        }
    }

	if (CurrentStrategy == Cautious) // Prioritise non-adjacent rooms in cautious strategy
    {
        ARoomVolume* PlayerCurrentRoom = Cast<ARoomVolume>(BlackboardComp->GetValueAsObject("PlayerCurrentRoom"));

        if (PlayerCurrentRoom)
        {
            for (AActor* Actor : Rooms)
            {
				ARoomVolume* Room = Cast<ARoomVolume>(Actor);
                
                if (!IsValid(Room)) continue;
                if (Room == PlayerCurrentRoom) continue;
                if (PlayerCurrentRoom->ConnectedRooms.Contains(Room)) continue;

                if (Room->EnemyTimeInRoom < 10.0f)
                {
                    TargetRoom = Room;
                    break;
                }
			}

            if (!TargetRoom)
            {
                for (AActor* Actor : Rooms)
                {
                    ARoomVolume* Room = Cast<ARoomVolume>(Actor);
                    if (!IsValid(Room)) continue;

                    if (Room == PlayerCurrentRoom) continue;
                    if (PlayerCurrentRoom->ConnectedRooms.Contains(Room)) continue;

                    TargetRoom = Room;
                    break;
                }
            }
        }
	}
    */

    FNavLocation RandomLocation;

    if (GetRandomLocationInRoom(CurrentRoom, NavSys, RandomLocation))
    {
        BlackboardComp->SetValueAsVector(GetSelectedBlackboardKey(), RandomLocation.Location);
		BlackboardComp->SetValueAsObject("TargetRoom", CurrentRoom);

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
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Actor %s is not a valid ARoomVolume in UBTTask_Search::GetAllRooms"), *Actor->GetName());
        }
    }
}

bool UBTTask_Search::GetRandomLocationInRoom(ARoomVolume* Room, UNavigationSystemV1* NavSys, FNavLocation& OutLocation)
{
    if (!Room || !NavSys) return false;

    FVector Origin, BoxExtent;
    Room->GetActorBounds(false, Origin, BoxExtent);

    constexpr int32 MaxAttempts = 10;

    for (int32 i = 0; i < MaxAttempts; i++)
    {
        FVector RandomPoint(
            FMath::FRandRange(Origin.X - BoxExtent.X, Origin.X + BoxExtent.X),
            FMath::FRandRange(Origin.Y - BoxExtent.Y, Origin.Y + BoxExtent.Y),
            Origin.Z
        );

        FNavLocation ProjectedLocation;

        if (NavSys->ProjectPointToNavigation(
            RandomPoint,
            ProjectedLocation,
            FVector(50.f, 50.f, BoxExtent.Z)))
        {
            if (Room->EncompassesPoint(ProjectedLocation.Location))
            {
                OutLocation = ProjectedLocation;
                return true;
            }
        }
    }

    return false;
}

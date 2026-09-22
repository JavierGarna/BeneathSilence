// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_RunAway.h"
#include "Kismet/GameplayStatics.h"
#include "RoomVolume.h"
#include "EnemyAIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_RunAway::UBTTask_RunAway()
{
	NodeName = "Run Away";
}

EBTNodeResult::Type UBTTask_RunAway::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComponent, NodeMemory);

    AAIController* AIController = OwnerComponent.GetAIOwner();
    if (!AIController)
    {
        UE_LOG(LogTemp, Warning, TEXT("AIController is null in UBTTask_RunAway::ExecuteTask"));
        return EBTNodeResult::Failed;
    }

    APawn* AIPawn = AIController->GetPawn();
    if (!AIPawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("AIPawn is null in UBTTask_RunAway::ExecuteTask"));
        return EBTNodeResult::Failed;
    }

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(AIPawn->GetWorld());
    if (!NavSys)
    {
        UE_LOG(LogTemp, Warning, TEXT("Navigation System is null in UBTTask_RunAway::ExecuteTask"));
        return EBTNodeResult::Failed;
    }

    UBlackboardComponent* BlackboardComp = OwnerComponent.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("BlackboardComp is null in UBTTask_RunAway::ExecuteTask"));
        return EBTNodeResult::Failed;
    }

	GetAllRooms();
    ARoomVolume* CurrentRoom = Cast<ARoomVolume>(BlackboardComp->GetValueAsObject("PlayerCurrentRoom"));
	ARoomVolume* TargetRoom = nullptr;
	bool bFoundRoomToRunAway = false;

	for (ARoomVolume* Room : Rooms)
	{
        if (!IsValid(Room)) continue;
        if (Room == CurrentRoom) continue;
        if (CurrentRoom->ConnectedRooms.Contains(Room)) continue;
        if (Room->bIsLocked) continue;

		TargetRoom = Room;
		bFoundRoomToRunAway = true;
		break;
	}

	if (bFoundRoomToRunAway)
	{
        FNavLocation RandomLocation;
        if (GetRandomLocationInRoom(TargetRoom, NavSys, RandomLocation))
        {
            BlackboardComp->SetValueAsVector(GetSelectedBlackboardKey(), RandomLocation.Location);
            BlackboardComp->SetValueAsObject("TargetRoom", TargetRoom);

            if (TargetRoom->EnemyTimeInRoom >= 20.f)
            {
                for (ARoomVolume* Room : Rooms)
                {
                    if (!IsValid(Room)) continue;
                    Room->EnemyTimeInRoom = 0.f;
                }
            }

            return EBTNodeResult::Succeeded;
        }
    }
	else UE_LOG(LogTemp, Warning, TEXT("No room found to run away in UBTTask_RunAway::ExecuteTask"));

	return EBTNodeResult::Failed;
}

void UBTTask_RunAway::GetAllRooms()
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
            UE_LOG(LogTemp, Warning, TEXT("Actor %s is not a valid ARoomVolume in UBTTask_RunAway::GetAllRooms"), *Actor->GetName());
        }
    }
}

bool UBTTask_RunAway::GetRandomLocationInRoom(ARoomVolume* Room, UNavigationSystemV1* NavSys, FNavLocation& OutLocation)
{
    if (!Room || !NavSys) return false;

    // Log room and navsys for debugging
    UE_LOG(LogTemp, Warning, TEXT("Getting random location in room: %s, NavSys: %s"), *Room->GetName(), *NavSys->GetName());

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
            UE_LOG(LogTemp, Warning, TEXT("Projected location: %s"), *ProjectedLocation.Location.ToString());
            if (Room->EncompassesPoint(ProjectedLocation.Location))
            {
                OutLocation = ProjectedLocation;
                return true;
            }
            else UE_LOG(LogTemp, Warning, TEXT("Projected location %s is not within room bounds."), *ProjectedLocation.Location.ToString());
        }
        else UE_LOG(LogTemp, Warning, TEXT("Failed to project point %s to navigation."), *RandomPoint.ToString());
    }

    return false;
}

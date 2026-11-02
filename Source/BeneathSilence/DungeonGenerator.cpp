// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonGenerator.h"
#include "RoomBase.h"
#include "DrawDebugHelpers.h"

// Sets default values
ADungeonGenerator::ADungeonGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADungeonGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	GenerateDungeon();
}

// Called every frame
void ADungeonGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADungeonGenerator::GenerateDungeon()
{
    if (!SpawnedRooms.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("Dungeon already generated."));
        return;
    }

    if (RoomClasses.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("No room classes assigned."));
        return;
    }

    TArray<FIntPoint> Layout = GenerateLayout();

    const FVector ReservedLocation = GetActorLocation() + FVector(RoomSize, 0, 0);

    SpawnRooms(Layout);
    ConnectRooms();

    UE_LOG(LogTemp, Warning, TEXT("Generated %d rooms."), SpawnedRooms.Num());
}

TArray<FIntPoint> ADungeonGenerator::GenerateLayout() const
{
    TArray<FIntPoint> Layout;

    const int32 TargetRooms = FMath::Max(1, NumberOfRooms);

    const FIntPoint StartPosition(0, 0);
    const FIntPoint ReservedPosition(1, 0); // Existing room to the east

    TSet<FIntPoint> Occupied;
    Occupied.Add(StartPosition);
    Occupied.Add(ReservedPosition);

    Layout.Add(StartPosition);

    const TArray<FIntPoint> Directions = 
    {
        FIntPoint(1, 0),
        FIntPoint(-1, 0),
        FIntPoint(0, 1),
        FIntPoint(0, -1)
    };

    while (Layout.Num() < TargetRooms)
    {
        // Choose an existing generated room
        const int32 RandomIndex = FMath::RandRange(0, Layout.Num() - 1);

        const FIntPoint CurrentPosition = Layout[RandomIndex];

        // Find all available neighbouring positions
        TArray<FIntPoint> AvailablePositions;

        for (const FIntPoint& Direction : Directions)
        {
            const FIntPoint NextPosition = CurrentPosition + Direction;

            if (!Occupied.Contains(NextPosition))
            {
                AvailablePositions.Add(NextPosition);
            }
        }

        // No available neighbours: try another room
        if (AvailablePositions.IsEmpty())
        {
            continue;
        }

        const int32 NextIndex = FMath::RandRange(0, AvailablePositions.Num() - 1);

        const FIntPoint NewPosition =   AvailablePositions[NextIndex];

        Occupied.Add(NewPosition);
        Layout.Add(NewPosition);
    }

    return Layout;
}

void ADungeonGenerator::SpawnRooms(const TArray<FIntPoint>& Layout)
{
    if (RoomClasses.IsEmpty())
    {
        UE_LOG(LogTemp, Error,
            TEXT("DungeonGenerator: No room classes assigned!"));
        return;
    }

    UWorld* World = GetWorld();

    if (!World) return;

    for (const FIntPoint& GridPosition : Layout)
    {
        const int32 RandomIndex = FMath::RandRange(0, RoomClasses.Num() - 1);

        TSubclassOf<ARoomBase> SelectedRoom = RoomClasses[RandomIndex];

        if (!SelectedRoom) continue;

        const FVector SpawnLocation = GetActorLocation() + FVector(GridPosition.X * RoomSize, GridPosition.Y * RoomSize, 0.0f);
        const FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation);

        ARoomBase* NewRoom = World->SpawnActorDeferred<ARoomBase>(SelectedRoom, SpawnTransform, this);

        if (!NewRoom) continue;

        NewRoom->bDoorNorth = false;
        NewRoom->bDoorSouth = false;
        NewRoom->bDoorEast = false;
        NewRoom->bDoorWest = false;

        // Finish spawning after initialising the room.
        NewRoom->FinishSpawning(SpawnTransform);

        SpawnedRooms.Add(GridPosition, NewRoom);
    }
}

void ADungeonGenerator::ConnectRooms()
{
    const FIntPoint StartPosition(0, 0);

    for (const auto& Pair : SpawnedRooms)
    {
        const FIntPoint Position = Pair.Key;
        ARoomBase* Room = Pair.Value.Get();

        if (!IsValid(Room))
            continue;

        auto GetNeighbour = [&](const FIntPoint& Offset) -> ARoomBase*
            {
                const FIntPoint NeighbourPosition = Position + Offset;

                if (TObjectPtr<ARoomBase>* Found =
                    SpawnedRooms.Find(NeighbourPosition))
                {
                    return Found->Get();
                }

                return nullptr;
            };

        ARoomBase* North = GetNeighbour(FIntPoint(0, -1));
        ARoomBase* South = GetNeighbour(FIntPoint(0, 1));
        ARoomBase* East = GetNeighbour(FIntPoint(1, 0));
        ARoomBase* West = GetNeighbour(FIntPoint(-1, 0));

        Room->bDoorNorth = IsValid(North);
        Room->bDoorSouth = IsValid(South);
        Room->bDoorEast = IsValid(East);
        Room->bDoorWest = IsValid(West);

        // Only the first generated room connects
        // north to the existing room.
        if (Position == FIntPoint(0, 0))
        {
            Room->bDoorEast = true;
        }

        if (IsValid(North)) Room->AddConnectedRoom(North);
        if (IsValid(South)) Room->AddConnectedRoom(South);
        if (IsValid(East))  Room->AddConnectedRoom(East);
        if (IsValid(West))  Room->AddConnectedRoom(West);

        Room->UpdateWalls();

    }
}

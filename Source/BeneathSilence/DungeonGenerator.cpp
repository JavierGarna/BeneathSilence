// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonGenerator.h"
#include "RoomBase.h"
#include "DrawDebugHelpers.h"
#include "EnemyCharacter.h"

// Sets default values
ADungeonGenerator::ADungeonGenerator()
{
    // Set this actor to call Tick() every frame.
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

    TArray<FRoomPlacement> Layout = GenerateLayout();

    const FVector ReservedLocation = GetActorLocation() + FVector(RoomSize, 0, 0);

    SpawnRooms(Layout);
    ConnectRooms();

    UE_LOG(LogTemp, Warning, TEXT("Generated %d rooms."), Layout.Num());
}

TArray<FRoomPlacement> ADungeonGenerator::GenerateLayout() const
{
    TArray<FRoomPlacement> Layout;

    const int32 TargetRooms = FMath::Max(1, NumberOfRooms);

    const FIntPoint StartPosition(0, 0);
    const FIntPoint ReservedPosition(1, 0); // Existing garden to the east
    const FIntPoint ReservedDimensions(2, 2);

    TSet<FIntPoint> Occupied;
    Occupied.Add(StartPosition);

    // Reserve all cells occupied by the garden
    for (int32 X = 0; X < ReservedDimensions.X; X++)
    {
        for (int32 Y = 0; Y < ReservedDimensions.Y; Y++)
        {
            Occupied.Add(ReservedPosition + FIntPoint(X, Y));
        }
    }

    const TArray<FIntPoint> Directions =
    {
        FIntPoint(1, 0),
        FIntPoint(-1, 0),
        FIntPoint(0, 1),
        FIntPoint(0, -1)
    };

    if (RoomClasses.IsEmpty())
    {
        return Layout;
    }

    // Choose the starting room
    TSubclassOf<ARoomBase> StartRoomClass = RoomClasses[0];

    if (!StartRoomClass)
    {
        return Layout;
    }

    const ARoomBase* StartRoomDefaults = StartRoomClass->GetDefaultObject<ARoomBase>();

    // The starting room must occupy one grid cell
    if (StartRoomDefaults->RoomDimensions != FIntPoint(1, 1))
    {
        UE_LOG(LogTemp, Error, TEXT("The starting room must have dimensions (1,1)."));
        return Layout;
    }

    FRoomPlacement StartRoom;
    StartRoom.Position = StartPosition;
    StartRoom.RoomDimensions = FIntPoint(1, 1);
    StartRoom.RoomClass = StartRoomClass;

    Layout.Add(StartRoom);

    int32 Attempts = 0;
    const int32 MaxAttempts = TargetRooms * 100;

    while (Layout.Num() < TargetRooms && Attempts < MaxAttempts)
    {
        Attempts++;

        // Choose a random room class
        const int32 RoomClassIndex = FMath::RandRange(0, RoomClasses.Num() - 1);

        const TSubclassOf<ARoomBase> SelectedRoomClass = RoomClasses[RoomClassIndex];

        if (!SelectedRoomClass)
        {
            continue;
        }

        const ARoomBase* RoomDefaults = SelectedRoomClass->GetDefaultObject<ARoomBase>();

        const FIntPoint RoomDimensions = RoomDefaults->RoomDimensions;

        if (RoomDimensions.X < 1 || RoomDimensions.Y < 1)
        {
            continue;
        }

        // Choose an existing generated room
        const int32 RandomIndex = FMath::RandRange(0, Layout.Num() - 1);

        const FRoomPlacement& CurrentRoom = Layout[RandomIndex];

        // Find all available neighbouring positions
        TArray<FIntPoint> AvailablePositions;

        // Check every cell around the existing room
        for (int32 X = 0; X < CurrentRoom.RoomDimensions.X; X++)
        {
            for (int32 Y = 0; Y < CurrentRoom.RoomDimensions.Y; Y++)
            {
                const FIntPoint CurrentPosition = CurrentRoom.Position + FIntPoint(X, Y);

                for (const FIntPoint& Direction : Directions)
                {
                    const FIntPoint NextPosition = CurrentPosition + Direction;

                    // Align the new room so it connects to the existing room
                    for (int32 RoomX = 0; RoomX < RoomDimensions.X; RoomX++)
                    {
                        for (int32 RoomY = 0; RoomY < RoomDimensions.Y; RoomY++)
                        {
                            const FIntPoint NewPosition = NextPosition - FIntPoint(RoomX, RoomY);

                            if (CanPlaceRoom(NewPosition, RoomDimensions, Occupied))
                            {
                                const ARoomBase* CurrentRoomDefaults = CurrentRoom.RoomClass->GetDefaultObject<ARoomBase>();

                                const FIntPoint CurrentLocalCell = CurrentPosition - CurrentRoom.Position;
                                const FIntPoint NewLocalCell = NextPosition - NewPosition;

                                const FIntPoint OppositeDirection(-Direction.X, -Direction.Y);

                                if (CurrentRoomDefaults->CanConnectAt(CurrentLocalCell, Direction) &&
                                    RoomDefaults->CanConnectAt(NewLocalCell, OppositeDirection))
                                {
                                    AvailablePositions.AddUnique(NewPosition);
                                }
                            }
                        }
                    }
                }
            }
        }

        // No available neighbours: try another room
        if (AvailablePositions.IsEmpty())
        {
            continue;
        }

        const int32 NextIndex = FMath::RandRange(0, AvailablePositions.Num() - 1);

        const FIntPoint NewPosition = AvailablePositions[NextIndex];

        // Mark all cells occupied by the room
        for (int32 X = 0; X < RoomDimensions.X; X++)
        {
            for (int32 Y = 0; Y < RoomDimensions.Y; Y++)
            {
                Occupied.Add(NewPosition + FIntPoint(X, Y));
            }
        }

        FRoomPlacement NewRoom;
        NewRoom.Position = NewPosition;
        NewRoom.RoomDimensions = RoomDimensions;
        NewRoom.RoomClass = SelectedRoomClass;

        Layout.Add(NewRoom);
    }

    if (Layout.Num() < TargetRooms)
    {
        UE_LOG(LogTemp, Warning, TEXT("Could only generate %d out of %d rooms."), Layout.Num(), TargetRooms);
    }

    return Layout;
}

void ADungeonGenerator::SpawnRooms(const TArray<FRoomPlacement>& Layout)
{
    if (RoomClasses.IsEmpty())
    {
        UE_LOG(LogTemp, Error,
            TEXT("DungeonGenerator: No room classes assigned!"));
        return;
    }

    UWorld* World = GetWorld();

    if (!World) return;

    for (const FRoomPlacement& Room : Layout)
    {
        const FIntPoint GridPosition = Room.Position;
        const FIntPoint RoomDimensions = Room.RoomDimensions;

        TSubclassOf<ARoomBase> SelectedRoom = Room.RoomClass;

        if (!SelectedRoom) continue;

        // Calculate the room's world position
        FVector SpawnLocation = GetActorLocation() + FVector(
            GridPosition.X * RoomSize,
            GridPosition.Y * RoomSize,
            0.0f
        );

        if (RoomDimensions.X > 1 || RoomDimensions.Y > 1)
        {
            SpawnLocation.X += (RoomDimensions.X - 1) * RoomSize;
        }

        const FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation);

        ARoomBase* NewRoom = World->SpawnActorDeferred<ARoomBase>(SelectedRoom, SpawnTransform, this);

        if (!NewRoom) continue;

        NewRoom->bDoorNorth = false;
        NewRoom->bDoorSouth = false;
        NewRoom->bDoorEast = false;
        NewRoom->bDoorWest = false;

        // Finish spawning after initialising the room
        NewRoom->FinishSpawning(SpawnTransform);

        // Calculate the expected centre of the room
        const FVector ExpectedCentre = GetActorLocation() + FVector(
            GridPosition.X * RoomSize + (RoomDimensions.X - 1) * RoomSize * 0.5f,
            GridPosition.Y * RoomSize + (RoomDimensions.Y - 1) * RoomSize * 0.5f,
            0.0f
        );

        // Get the actual room bounds
        FVector BoundsOrigin;
        FVector BoundsExtent;

        NewRoom->GetActorBounds(false, BoundsOrigin, BoundsExtent);

        const FVector Offset = ExpectedCentre - BoundsOrigin;

        NewRoom->AddActorWorldOffset(FVector(Offset.X, Offset.Y, 0.0f));

        // Register every cell occupied by this room
        for (int32 X = 0; X < RoomDimensions.X; X++)
        {
            for (int32 Y = 0; Y < RoomDimensions.Y; Y++)
            {
                const FIntPoint Cell = GridPosition + FIntPoint(X, Y);

                SpawnedRooms.Add(Cell, NewRoom);
            }
        }
    }
}

void ADungeonGenerator::ConnectRooms()
{
    const FIntPoint StartPosition(0, 0);

    TSet<ARoomBase*> ProcessedRooms;

    // Reset all doors before connecting rooms
    for (const auto& Pair : SpawnedRooms)
    {
        ARoomBase* Room = Pair.Value.Get();

        if (!IsValid(Room) || ProcessedRooms.Contains(Room))
        {
            continue;
        }

        ProcessedRooms.Add(Room);

        Room->bDoorNorth = false;
        Room->bDoorSouth = false;
        Room->bDoorEast = false;
        Room->bDoorWest = false;
    }

    ProcessedRooms.Empty();

    for (const auto& Pair : SpawnedRooms)
    {
        ARoomBase* Room = Pair.Value.Get();

        if (!IsValid(Room) || ProcessedRooms.Contains(Room))
        {
            continue;
        }

        ProcessedRooms.Add(Room);

        // Find all cells occupied by this room
        TArray<FIntPoint> RoomCells;

        FIntPoint RoomPosition(INT32_MAX, INT32_MAX);

        for (const auto& CellPair : SpawnedRooms)
        {
            if (CellPair.Value.Get() == Room)
            {
                RoomCells.Add(CellPair.Key);

                RoomPosition.X = FMath::Min(RoomPosition.X, CellPair.Key.X);
                RoomPosition.Y = FMath::Min(RoomPosition.Y, CellPair.Key.Y);
            }
        }

        const TArray<FIntPoint> Directions =
        {
            FIntPoint(0, -1),
            FIntPoint(0, 1),
            FIntPoint(1, 0),
            FIntPoint(-1, 0)
        };

        // Check every neighbouring cell
        for (const FIntPoint& Cell : RoomCells)
        {
            const FIntPoint LocalCell = Cell - RoomPosition;

            for (const FIntPoint& Direction : Directions)
            {
                // Check whether this room has a door at this position
                if (!Room->CanConnectAt(LocalCell, Direction))
                {
                    continue;
                }

                const FIntPoint NeighbourCell = Cell + Direction;

                TObjectPtr<ARoomBase>* Found = SpawnedRooms.Find(NeighbourCell);

                if (!Found)
                {
                    continue;
                }

                ARoomBase* Neighbour = Found->Get();

                if (!IsValid(Neighbour) || Neighbour == Room)
                {
                    continue;
                }

                // Find the neighbouring room's anchor position
                FIntPoint NeighbourPosition(INT32_MAX, INT32_MAX);

                for (const auto& NeighbourPair : SpawnedRooms)
                {
                    if (NeighbourPair.Value.Get() == Neighbour)
                    {
                        NeighbourPosition.X = FMath::Min(NeighbourPosition.X, NeighbourPair.Key.X);
                        NeighbourPosition.Y = FMath::Min(NeighbourPosition.Y, NeighbourPair.Key.Y);
                    }
                }

                const FIntPoint NeighbourLocalCell = NeighbourCell - NeighbourPosition;

                // Check whether the neighbouring room has a matching door
                if (!Neighbour->CanConnectAt(NeighbourLocalCell, FIntPoint(-Direction.X, -Direction.Y)))
                {
                    continue;
                }

                // Enable the corresponding doors
                if (Direction == FIntPoint(0, -1))
                {
                    Room->bDoorNorth = true;
                    Neighbour->bDoorSouth = true;
                }
                else if (Direction == FIntPoint(0, 1))
                {
                    Room->bDoorSouth = true;
                    Neighbour->bDoorNorth = true;
                }
                else if (Direction == FIntPoint(1, 0))
                {
                    Room->bDoorEast = true;
                    Neighbour->bDoorWest = true;
                }
                else if (Direction == FIntPoint(-1, 0))
                {
                    Room->bDoorWest = true;
                    Neighbour->bDoorEast = true;
                }

                Room->AddConnectedRoom(Neighbour);
            }
        }
    }

    // Keep the starting room connected to the existing room to the east
    if (TObjectPtr<ARoomBase>* Found = SpawnedRooms.Find(StartPosition))
    {
        if (IsValid(Found->Get()))
        {
            Found->Get()->bDoorEast = true;
        }
    }

    // Update walls after all connections have been established
    for (ARoomBase* Room : ProcessedRooms)
    {
        if (IsValid(Room))
        {
            Room->UpdateWalls();
        }
    }
}

bool ADungeonGenerator::CanPlaceRoom(FIntPoint Position, FIntPoint RoomDimensions, const TSet<FIntPoint>& Occupied) const
{
    for (int32 X = 0; X < RoomDimensions.X; X++)
    {
        for (int32 Y = 0; Y < RoomDimensions.Y; Y++)
        {
            const FIntPoint Cell = Position + FIntPoint(X, Y);

            if (Occupied.Contains(Cell))
            {
                return false;
            }
        }
    }

    return true;
}

void ADungeonGenerator::SpawnEnemy()
{
    if (!EnemyClass || !GetWorld() || SpawnedRooms.IsEmpty())
        return;

    // Prevent spawning multiple enemies
    if (IsValid(Enemy)) return;

    ARoomBase* Room = SpawnedRooms[FMath::RandRange(1, SpawnedRooms.Num() - 1)];

    if (!IsValid(Room)) return;

    FVector SpawnLocation = Room->GetActorLocation() + FVector(0, 0, 100);

    Enemy = GetWorld()->SpawnActor<AEnemyCharacter>(EnemyClass, SpawnLocation, FRotator::ZeroRotator);
}

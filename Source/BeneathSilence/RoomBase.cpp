// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomBase.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"

#include "PlayerCharacter.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "FixedCameraManager.h"
#include "FixedCamera.h"

#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

// Sets default values
ARoomBase::ARoomBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    RoomTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("RoomTrigger"));
    RoomTrigger->SetupAttachment(RootComponent);
    RoomTrigger->SetBoxExtent(FVector(500.0f, 500.0f, 200.0f));
    RoomTrigger->SetCollisionProfileName(TEXT("Trigger"));

    RoomCamera = CreateDefaultSubobject<UChildActorComponent>(TEXT("RoomCamera"));
	RoomCamera->SetupAttachment(RootComponent);
    RoomCamera->SetChildActorClass(AFixedCamera::StaticClass());

    RoomTrigger->OnComponentBeginOverlap.AddDynamic(this, &ARoomBase::HandleBeginOverlap);

    NorthWall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NorthWall"));
    NorthWall->SetupAttachment(RootComponent);

    SouthWall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SouthWall"));
    SouthWall->SetupAttachment(RootComponent);

    EastWall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EastWall"));
    EastWall->SetupAttachment(RootComponent);

    WestWall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WestWall"));
    WestWall->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ARoomBase::BeginPlay()
{
	Super::BeginPlay();
	
    Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    Enemy = Cast<AEnemyCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyCharacter::StaticClass()));

    if (Enemy)
    {
        EnemyAIController = Cast<AEnemyAIController>(Enemy->GetController());
    }
}

// Called every frame
void ARoomBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (!EnemyAIController) return;

    UBlackboardComponent* BlackboardComp = EnemyAIController->GetBlackboardComponent();

    if (!BlackboardComp) return;

    if (BlackboardComp->GetValueAsObject("PlayerCurrentRoom") == this)
    {
        PlayerTimeInRoom += DeltaTime;
    }

    if (BlackboardComp->GetValueAsObject("EnemyCurrentRoom") == this)
    {
        EnemyTimeInRoom += DeltaTime;
    }
}

void ARoomBase::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    UpdateWalls();
}

void ARoomBase::AddConnectedRoom(ARoomBase* Room)
{
    if (!IsValid(Room) || Room == this) return;

    ConnectedRooms.AddUnique(Room);
    Room->ConnectedRooms.AddUnique(this);
}

void ARoomBase::HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor) return;
    if (!IsValid(EnemyAIController)) return;

    UBlackboardComponent* BlackboardComp = EnemyAIController->GetBlackboardComponent();

	if (!BlackboardComp) return;

    if (OtherActor == Enemy)
    {
        BlackboardComp->SetValueAsObject("EnemyCurrentRoom", this);
    }
    else if (OtherActor == Player)
    {
        BlackboardComp->SetValueAsObject("PlayerCurrentRoom", this);

        APlayerController* PlayerController = Cast<APlayerController>(Player->GetController());

        if (PlayerController && RoomCamera)
        {
            AFixedCameraManager* CameraManager = Cast<AFixedCameraManager>(PlayerController->PlayerCameraManager);

            if (CameraManager)
            {
                AFixedCamera* Camera = Cast<AFixedCamera>(RoomCamera->GetChildActor());

                if (Camera)
                {
                    CameraManager->SwitchToFixedCamera(Camera);
                }
            }
        }
    }
    else
    {
        return;
    }

    UpdateRoomAdjacency();
}

void ARoomBase::UpdateRoomAdjacency()
{
    if (!EnemyAIController) return;

    UBlackboardComponent* BlackboardComp = EnemyAIController->GetBlackboardComponent();

    if (!BlackboardComp) return;

    ARoomBase* PlayerRoom = Cast<ARoomBase>(BlackboardComp->GetValueAsObject("PlayerCurrentRoom"));

    ARoomBase* EnemyRoom = Cast<ARoomBase>(BlackboardComp->GetValueAsObject("EnemyCurrentRoom"));

    bool bIsAdjacent = false;

    if (PlayerRoom && EnemyRoom)
    {
        bIsAdjacent = PlayerRoom->ConnectedRooms.Contains(EnemyRoom);
    }

    BlackboardComp->SetValueAsBool("IsAdjacentToPlayerRoom", bIsAdjacent);
}

bool ARoomBase::CanConnectAt(FIntPoint LocalCell, FIntPoint Direction) const
{
    if (Direction == FIntPoint(0, -1))
    {
        return LocalCell.X == RoomDimensions.X - 1 &&
            LocalCell.Y == 0;
    }

    if (Direction == FIntPoint(0, 1))
    {
        return LocalCell.X == RoomDimensions.X - 1 &&
            LocalCell.Y == RoomDimensions.Y - 1;
    }

    if (Direction == FIntPoint(1, 0))
    {
        return LocalCell.X == RoomDimensions.X - 1 &&
            LocalCell.Y == 0;
    }

    if (Direction == FIntPoint(-1, 0))
    {
        return LocalCell.X == 0 &&
            LocalCell.Y == 0;
    }

    return false;
}

void ARoomBase::UpdateWalls()
{
    if (!SolidWallMesh || !DoorWallMesh)
        return;

    NorthWall->SetStaticMesh(bDoorNorth ? DoorWallMesh : SolidWallMesh);
    SouthWall->SetStaticMesh(bDoorSouth ? DoorWallMesh : SolidWallMesh);
    EastWall->SetStaticMesh(bDoorEast ? DoorWallMesh : SolidWallMesh);
    WestWall->SetStaticMesh(bDoorWest ? DoorWallMesh : SolidWallMesh);
}


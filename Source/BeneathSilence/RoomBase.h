// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomBase.generated.h"

class UBoxComponent;
class APlayerCharacter;
class AEnemyCharacter;
class AEnemyAIController;

UCLASS()
class BENEATHSILENCE_API ARoomBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoomBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	// Rooms connected to this one
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	TArray<ARoomBase*> ConnectedRooms;

	// Time spent inside room
	UPROPERTY(BlueprintReadOnly, Category = "Room")
	float PlayerTimeInRoom = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Room")
	float EnemyTimeInRoom = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Doors")
	bool bDoorNorth = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Doors")
	bool bDoorSouth = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Doors")
	bool bDoorEast = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Doors")
	bool bDoorWest = false;

	UFUNCTION(BlueprintCallable, Category = "Room")
	void AddConnectedRoom(ARoomBase* Room);

	UFUNCTION(BlueprintCallable, Category = "Walls")
	void UpdateWalls();

protected:
	// Root component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room")
	USceneComponent* SceneRoot;

	// Detects player and enemy entering the room
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room")
	UBoxComponent* RoomTrigger;

	// Camera belonging to this room
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UChildActorComponent* RoomCamera;

	UPROPERTY()
	APlayerCharacter* Player = nullptr;

	UPROPERTY()
	AEnemyCharacter* Enemy = nullptr;

	UPROPERTY()
	AEnemyAIController* EnemyAIController = nullptr;

	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void UpdateRoomAdjacency();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Walls")
	UStaticMeshComponent* NorthWall;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Walls")
	UStaticMeshComponent* SouthWall;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Walls")
	UStaticMeshComponent* EastWall;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Walls")
	UStaticMeshComponent* WestWall;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Walls")
	UStaticMesh* SolidWallMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Walls")
	UStaticMesh* DoorWallMesh;

};

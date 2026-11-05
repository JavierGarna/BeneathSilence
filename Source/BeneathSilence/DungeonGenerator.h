// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonGenerator.generated.h"

class ARoomBase;

USTRUCT(BlueprintType)
struct FRoomPlacement
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FIntPoint Position = FIntPoint(0, 0);
	UPROPERTY()
	FIntPoint RoomDimensions = FIntPoint(1, 1);
	UPROPERTY()
	TSubclassOf<ARoomBase> RoomClass;
};

UCLASS()
class BENEATHSILENCE_API ADungeonGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADungeonGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon")
	int32 NumberOfRooms = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon")
	float RoomSize = 800.0f;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeon")
	TArray<TSubclassOf<ARoomBase>> RoomClasses;

	UFUNCTION(BlueprintCallable, Category = "Dungeon")
	void GenerateDungeon();

private:
	UPROPERTY()
	TMap<FIntPoint, TObjectPtr<ARoomBase>> SpawnedRooms;

	TArray<FRoomPlacement> GenerateLayout() const;

	void SpawnRooms(const TArray<FRoomPlacement>& Layout);

	void ConnectRooms();

	bool CanPlaceRoom(FIntPoint Position, FIntPoint RoomDimensions, const TSet<FIntPoint>& Occupied) const;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyAIController.generated.h"

class ARoomVolume;
struct FAIStimulus;
class UBehaviorTree;
class UBlackboardComponent;
class APlayerCharacter;


enum EEnemyState
{
	Search,
	Investigate,
	Chase
};

enum EEnemyStrategy
{
	Normal,
	Aggressive,
	Cautious,
};

USTRUCT(BlueprintType)
struct FEnemyLearningData
{
	GENERATED_BODY()

	UObject* SelfActor = nullptr;
	uint8 CurrentState = Search;
	uint8 CurrentStrategy = Normal;

	UObject* TargetActor = nullptr;
	FVector TargetLocation = FVector::ZeroVector;
	ARoomVolume* TargetRoom = nullptr;

	FVector LastKnownPlayerLocation = FVector::ZeroVector;
	int LastKnownPlayerRoom = 0;
	float TimeSinceLastSeen = 9999.0f;
	float ConfidenceLevel = 0.0f;

	float LastStimulusStrength = 0.0f;
	FVector LastStimulusLocation = FVector::ZeroVector;
	float TimeSinceLastStimulus = 9999.0f;
	bool HasHeardPlayer = false;

	float CurrentDistanceToPlayer = 0.0f;
	bool IsAdjacentToPlayerRoom = false;

	float DesiredTensionLevel = 0.0f;
	float CurrentTensionLevel = 0.0f;
};

/**
 * 
 */
UCLASS()
class BENEATHSILENCE_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FEnemyLearningData GetLearningData();
	TArray<float> GetEnemyTimeInRooms();
	TArray<float> GetPlayerTimeInRooms();
	ARoomVolume* GetPlayerCurrentRoom();

	void SetCurrentState(FName NewState);
	void SetEnemyCurrentRoom(ARoomVolume* NewRoom);
	void SetPlayerCurrentRoom(ARoomVolume* NewRoom);


private:
	UPROPERTY(EditAnywhere)
	UBehaviorTree* EnemyBehaviorTree;

	UPROPERTY(EditAnywhere)
	UBlackboardComponent* BlackboardComp;
	UPROPERTY(EditAnywhere)
	APlayerCharacter* Player;

	UPROPERTY(EditAnywhere)
	ARoomVolume* EnemyCurrentRoom;
	UPROPERTY(EditAnywhere)
	ARoomVolume* PlayerCurrentRoom;
	UPROPERTY(EditAnywhere)
	TArray<float> EnemyTimeInRooms;
	UPROPERTY(EditAnywhere)
	TArray<float> PlayerTimeInRooms;

	UPROPERTY()
	int HasHeardPlayerTimer = 0;

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class ARoomVolume;
struct FAIStimulus;

USTRUCT(BlueprintType)
struct FEnemyLearningData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float PlayerNoiseLevel = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	FVector PlayerPosition = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	FName CurrentState = "Search";
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ARoomVolume*> Rooms;

	UFUNCTION()
	FEnemyLearningData GetLearningData();

	UFUNCTION()
	void SetCurrentState(FName NewState);

private:
	UPROPERTY(EditAnywhere)
	UBehaviorTree* EnemyBehaviorTree;

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};

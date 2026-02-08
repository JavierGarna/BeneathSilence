// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class ARoomVolume;

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

private:
	UPROPERTY(EditAnywhere)
	UBehaviorTree* EnemyBehaviorTree;
};

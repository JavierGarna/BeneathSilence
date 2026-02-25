// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomVolume.h"
#include "FixedCameraManager.h"
#include "PlayerCharacter.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"

ARoomVolume::ARoomVolume()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    OnActorBeginOverlap.AddDynamic(this, &ARoomVolume::HandleBeginOverlap);
}

void ARoomVolume::HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
    if (OtherActor == nullptr) return;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyCharacter::StaticClass()));
    AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(Enemy->GetController());;

    if (OtherActor->IsA(APlayerCharacter::StaticClass()))
    {
        ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);

        if (!PlayerCharacter || !RoomCamera) return;

        APlayerController* PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());
        if (!PlayerController) return;

        AFixedCameraManager* CameraManager = Cast<AFixedCameraManager>(PlayerController->PlayerCameraManager);
        if (CameraManager)
        {
            CameraManager->SwitchToFixedCamera(RoomCamera);
            if (EnemyAIController) EnemyAIController->SetPlayerCurrentRoom(this);
        }
    }
    else if (OtherActor->IsA(AEnemyCharacter::StaticClass()))
    {
        if (EnemyAIController) EnemyAIController->SetEnemyCurrentRoom(this);
    }
}


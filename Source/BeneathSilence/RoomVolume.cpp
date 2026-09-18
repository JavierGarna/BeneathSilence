// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomVolume.h"
#include "FixedCameraManager.h"
#include "PlayerCharacter.h"
#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

ARoomVolume::ARoomVolume()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    OnActorBeginOverlap.AddDynamic(this, &ARoomVolume::HandleBeginOverlap);
}

void ARoomVolume::HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
    if (OtherActor == nullptr) return;
    AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyCharacter::StaticClass()));
    ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
    AAIController* AIController = Cast<AAIController>(EnemyCharacter->GetController());
    APlayerController* PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());
    UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();

    if (EnemyCharacter)
    {
        if (AIController)
        {
            if (BlackboardComp)
            {
                if (OtherActor->IsA(AEnemyCharacter::StaticClass()))
                {
					BlackboardComp->SetValueAsObject("EnemyCurrentRoom", this);

                    for (ARoomVolume* ConnectedRoom : ConnectedRooms)
                    {
                        if (ConnectedRoom == BlackboardComp->GetValueAsObject("PlayerCurrentRoom"))
                        {
                            BlackboardComp->SetValueAsBool("IsAdjacentToPlayerRoom", true);
                        }
                    }
                }
            }
        }
    }

    if (PlayerCharacter)
    {
        if (!RoomCamera) return;

        if (PlayerController)
        {
            AFixedCameraManager* CameraManager = Cast<AFixedCameraManager>(PlayerController->PlayerCameraManager);
            if (CameraManager)
            {
                CameraManager->SwitchToFixedCamera(RoomCamera);
            }

			BlackboardComp->SetValueAsObject("PlayerCurrentRoom", this);

			for (ARoomVolume* ConnectedRoom : ConnectedRooms)
			{
				if (ConnectedRoom == BlackboardComp->GetValueAsObject("EnemyCurrentRoom"))
				{
					BlackboardComp->SetValueAsBool("IsAdjacentToPlayerRoom", true);
				}
			}
        }
    }
}

void ARoomVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
    // Get EnemyCharacter
    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyCharacter::StaticClass()));

    if (Enemy)
    {
		AAIController* AIController = Cast<AAIController>(Enemy->GetController());

        if (AIController)
        {
            UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();

            if (BlackboardComp)
            {
                UObject* PlayerCurrentRoom = BlackboardComp->GetValueAsObject("PlayerCurrentRoom");
                if (this == PlayerCurrentRoom)
                {
                    PlayerTimeInRoom += DeltaTime;
                }

				UObject* EnemyCurrentRoom = BlackboardComp->GetValueAsObject("EnemyCurrentRoom");

                if (this == EnemyCurrentRoom)
                {
                    EnemyTimeInRoom += DeltaTime;
                }
            }
        }
    }
}


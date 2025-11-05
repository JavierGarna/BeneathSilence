// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraTriggerVolume.h"
#include "FixedCameraManager.h"
#include "GameFramework/Character.h"

// Sets default values
ACameraTriggerVolume::ACameraTriggerVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    RootComponent = TriggerBox;

    TriggerBox->SetBoxExtent(FVector(200.0f, 200.0f, 100.0f));
    TriggerBox->SetCollisionProfileName(TEXT("Trigger"));

    BlendTime = 0.5f;
}

// Called when the game starts or when spawned
void ACameraTriggerVolume::BeginPlay()
{
	Super::BeginPlay();
	
    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ACameraTriggerVolume::OnOverlapBegin);
}

void ACameraTriggerVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
    if (!PlayerCharacter || !TriggerCamera)
    {
        return;
    }

    APlayerController* PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());
    if (!PlayerController)
    {
        return;
    }

    AFixedCameraManager* CameraManager = Cast<AFixedCameraManager>(PlayerController->PlayerCameraManager);
    if (CameraManager)
    {
        CameraManager->SwitchToFixedCamera(TriggerCamera, BlendTime);
    }
}

// Called every frame
void ACameraTriggerVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


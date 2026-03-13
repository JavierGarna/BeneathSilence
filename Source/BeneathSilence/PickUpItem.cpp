// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpItem.h"
#include "MainGameModeBase.h"
#include "PlayerCharacter.h"

// Sets default values
APickUpItem::APickUpItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Mesh"));
	ItemMesh->SetupAttachment(RootComponent);
	
	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger Sphere"));
	TriggerSphere->SetupAttachment(ItemMesh);

	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &APickUpItem::OnOverlapBegin);
}

// Called when the game starts or when spawned
void APickUpItem::BeginPlay()
{
	Super::BeginPlay();

}

void APickUpItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMainGameModeBase* GameMode = GetWorld() ? Cast<AMainGameModeBase>(GetWorld()->GetAuthGameMode()) : nullptr;
	if (GameMode)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
		// if overlapping actor is player, collect item
		if (Player)
		{
			GameMode->CollectItem();
			Destroy();
		}
	}
}

// Called every frame
void APickUpItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//FRotator Rotation(0.f, 90.f * DeltaTime, 0.f);
	//ItemMesh->AddLocalRotation(Rotation);

}


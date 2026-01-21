// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemClass.h"

#include "InventoryComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AItemClass::AItemClass()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Scene Root"));
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));

	CollisionBox->InitBoxExtent(FVector(150.0f, 150.0f, 150.0f));
	CollisionBox->SetGenerateOverlapEvents(true);
	
	RootComponent = MyMesh;
	CollisionBox->SetupAttachment(RootComponent);

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AItemClass::OnBoxBeginOverlap);
}

// Item collided with - to pickup
void AItemClass::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bSweep, const FHitResult& SweepResult)
{
	AActor* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (OtherActor != PlayerPawn) // If the collision wasn't with the player
		return;

	UInventoryComponent* PlayerInventory = OtherActor->FindComponentByClass<UInventoryComponent>();

	if(PlayerInventory) // Checks if the player has an inventory
	{
		if (PlayerInventory->AddItem(ItemID)) // If adding the item was successful
			Destroy();
	}
}

// Called when the game starts or when spawned
void AItemClass::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AItemClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
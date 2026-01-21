// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/BoxComponent.h>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "ItemClass.generated.h"

class AItemClass;

// Defines an inventory slot - not the item
USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (ToolTip = "The ID used to identify the item."))
	FName ItemID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory", meta = (ToolTip = "The quantity of a specific item."))
	int32 Quantity = 0;
};

// Defines an item
USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	// Displayed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Data", meta = (ToolTip = "The name displayed to the player in the Inventory list and preview."))
	FText ItemName = FText::FromString("My Item");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Data", meta = (ToolTip = "The description displayed to the player in the Inventory list and preview."))
	FText ItemDesc = FText::FromString("This is an item.");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Data", meta = (ToolTip = "The weight of the item is displayed to the player in the Inventory list and preview. It is also used for inventory sorting"))
	float ItemWeight = 1.f;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category="Item Data", meta = (ToolTip = "The value of the item is displayed to the player in the Inventory list and preview."))
	float ItemValue = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data", meta = (ToolTip = "The icon displayed to the player in the Inventory list and preview."))
	UTexture2D* ItemIcon;

	// Not Displayed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data", meta = (ToolTip = "The maximum number of this item that can be stacked in a single inventory slot."))
	int32 MaxStackSize = 2;

	UPROPERTY( EditAnywhere, Category="Item Data", meta = (ToolTip = "The specific blueprint actor to spawn in the world, when an item is dropped."))
	TSubclassOf<AItemClass> ItemActorClass;

	UPROPERTY(EditAnywhere, Category = "Item Data", meta = (ToolTip = "The asset used the item when displayed in the world."))
	UStaticMesh* ItemMesh;
};
UCLASS(Blueprintable,BlueprintType)
class INVENTORYSYSTEMPLUGIN_API AItemClass : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemClass();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Data")
	FName ItemID;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MyMesh;

	// When collided with, to pickup
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bSweep, const FHitResult& SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UBoxComponent* CollisionBox;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
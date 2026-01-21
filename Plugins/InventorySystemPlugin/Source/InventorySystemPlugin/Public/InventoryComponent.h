// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemClass.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INVENTORYSYSTEMPLUGIN_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", meta = (ToolTip = "The Data Table containing all item definitions (Stats, Icons, Meshes)."))
	class UDataTable* ItemDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", meta = (ToolTip = "The maximum total weight the owner can hold."))
	float MaxCarryWeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ToolTip = "The array of Inventory Slots (Item ID + Quantity)."))
	TArray<FInventorySlot> InventoryContent;

	// Helper function to merge the arrays
	UFUNCTION()
	void MergeArrays(TArray<FInventorySlot>& InSlots, int32 Left, int32 Mid, int32 Right);
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable, Category = "Inventory", meta = (ToolTip = "Fired when the inventory is updated in any way, used for UI updates."))
	FOnInventoryUpdated OnInventoryUpdated;

	UFUNCTION(BlueprintCallable, Category = "Inventory", meta = (ToolTip = "Attempts to add an item to the inventory. Handles stacking automatically."))
	bool AddItem(FName ItemID, int32 Amount = 1); // Allows more than one item to be added at a time

	UFUNCTION(BlueprintCallable, Category = "Inventory", meta = (ToolTip = "Removes a specific quantity of an item. If quantity reaches 0, the slot is cleared."))
	void RemoveItem(FName ItemID, int32 AmountToRemove = 1); // Allows more than one item to be removed at a time
	
	UFUNCTION(BlueprintCallable, Category = "Inventory", meta = (ToolTip = "Retrieves the item data struct for a given item, done so through the data table using IDs."))
	bool GetItemData(FName ItemID, FItemData& OutItemData) const; // Retrieves item data

	UFUNCTION(BlueprintCallable, Category = "Inventory", meta = (ToolTip = "Calculates the combined weight of all items currently in the inventory. "))
	float GetCurrentWeight() const; // Gets the weight of everything in the inventory

	UFUNCTION(BlueprintCallable, Category = "Inventory", meta = (ToolTip = "Removes the item from the array, and spawns it in the world - dropping it."))
	void DropItem(FName ItemID); // Drops the item, calls the remove function

	// Iterative Merge Sort - to organise inventory based on weight
	UFUNCTION(BlueprintCallable, Category = "Inventory", meta = (ToolTip = "Sorts the inventory by weight, using an iterative merge sort."))
	void SortInventory();
};
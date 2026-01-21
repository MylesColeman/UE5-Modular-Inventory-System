// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Math/UnrealMathUtility.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...

	MaxCarryWeight = 150.0f;
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Merges the sorted arrays
void UInventoryComponent::MergeArrays(TArray<FInventorySlot>& InSlots, int32 Left, int32 Mid, int32 Right)
{
	// Calculates the sizes of the two temporary subarrays
	int32 NumLeft = Mid - Left + 1;
	int32 NumRight = Right - Mid;

	// Creates temporary arrays to hold the split data
	TArray<FInventorySlot> LeftArray;
	LeftArray.SetNum(NumLeft);
	TArray<FInventorySlot> RightArray;
	RightArray.SetNum(NumRight);

	// Copies data from the main inventory array into the temporary subarrays
	for (int32 i = 0; i < NumLeft; i++)
		LeftArray[i] = InSlots[Left + i];
	for (int32 j = 0; j < NumRight; j++)
		RightArray[j] = InSlots[Mid + 1 + j];

	int32 i = 0;
	int32 j = 0;
	int k = Left; // Initial index of the merged subarray

	FItemData LeftItemData;
	FItemData RightItemData;
	
	// Loops as long as there are elements in both subarrays to compare
	while (i < NumLeft && j < NumRight)
	{
		// Gets the data for comparison
		GetItemData(LeftArray[i].ItemID, LeftItemData);
		GetItemData(RightArray[j].ItemID, RightItemData);
		
		// Comapres weight, to decide order
		if (LeftItemData.ItemWeight <= RightItemData.ItemWeight) // Left item lighter
		{
			InSlots[k] = LeftArray[i];
			i++;
		}
		else // Right item lighter
		{
			InSlots[k] = RightArray[j];
			j++;
		}
		k++;
	}

	// Copies remaining elements over - if one array finished earlier
	while (i < NumLeft) // Left array
	{
		InSlots[k] = LeftArray[i];
		i++;
		k++;
	}
	while (j < NumRight) // Right array
	{
		InSlots[k] = RightArray[j];
		j++;
		k++;
	}
}

// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UInventoryComponent::AddItem(FName ItemID, int32 Amount)
{
	// Item can't be added
	if (ItemID.IsNone() || !ItemDataTable || Amount <= 0) { return false; }

	FItemData ItemInfo;
	if (!GetItemData(ItemID, ItemInfo)) { return false; } // Failed to get item data

	float AdditionalWeight = ItemInfo.ItemWeight * Amount;
	if (GetCurrentWeight() + AdditionalWeight > MaxCarryWeight) // Inventory full
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot add item: %s max carry weight reached!"), *ItemID.ToString());
		return false;
	}

	int32 RemainingAmount = Amount;

	// Loops through the slots
	for (FInventorySlot& Slot : InventoryContent)
	{
		// Checks if the current slot is the same item ID as the one being checked, and that its not full
		if (Slot.ItemID == ItemID && Slot.Quantity < ItemInfo.MaxStackSize)
		{
			int32 SpaceInSlot = ItemInfo.MaxStackSize - Slot.Quantity; // The amount of space left in an item slot
			int32 AmountToAdd = FMath::Min(RemainingAmount, SpaceInSlot); // The amount that will be added to this slot

			Slot.Quantity += AmountToAdd; // Adds items to the slot count
			RemainingAmount -= AmountToAdd; // Calculates whats left to add

			if (RemainingAmount <= 0) // Slot full
				break;
		}
	}

	// Stops searching once there's no new items to handle
	while (RemainingAmount > 0)
	{
		int32 QuantityForNewSlot = FMath::Min(RemainingAmount, ItemInfo.MaxStackSize); // How many items will go in the new slot

		// Creates the new slot
		FInventorySlot NewSlot;
		NewSlot.ItemID = ItemID;
		NewSlot.Quantity = QuantityForNewSlot;

		// Adds the item to the new slot
		InventoryContent.Add(NewSlot);
		RemainingAmount -= QuantityForNewSlot;
	}

	OnInventoryUpdated.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("Added Item: %s!"), *ItemID.ToString());
	return true;
}

void UInventoryComponent::RemoveItem(FName ItemID, int32 AmountToRemove)
{
	// Invalid item, or the amount to remove is 0 or less
	if (ItemID.IsNone() || AmountToRemove <= 0) { return; }


	// Loops through the inventory backwards, starting with the least filled stacks
	for (int32 i = InventoryContent.Num() - 1; i >= 0; --i)
	{
		// Checks if the current item is equal to the item to be removed
		if (InventoryContent[i].ItemID == ItemID)
		{
			// Calculates how much can be taken from this specific slot
			int32 AmountTaken = FMath::Min(InventoryContent[i].Quantity, AmountToRemove);

			// Removes items from stack
			InventoryContent[i].Quantity -= AmountTaken;
			AmountToRemove -= AmountTaken;

			// Removes empty slots
			if (InventoryContent[i].Quantity <= 0)
				InventoryContent.RemoveAt(i);

			// Stops once all items removed
			if (AmountToRemove <= 0)
				break;
		}
	}

	OnInventoryUpdated.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("Removed Item: %s!"), *ItemID.ToString())
}

bool UInventoryComponent::GetItemData(FName ItemID, FItemData& OutItemData) const
{
	if (!ItemDataTable) // Couldn't retrieve the item data table, for item data
		return false;

	FItemData* Row = ItemDataTable->FindRow<FItemData>(ItemID, TEXT(""));
	if (Row) // Gets the item data required
	{
		OutItemData = *Row;
		return true;
	}

	return false; // Couldn't find item data
}

float UInventoryComponent::GetCurrentWeight() const
{
	float TotalWeight = 0.0f;

	// Loops through the item slots
	for (const FInventorySlot& Slot : InventoryContent)
	{
		FItemData ItemData;
		if (GetItemData(Slot.ItemID, ItemData)) // Gets the item
		{
			TotalWeight += (ItemData.ItemWeight * Slot.Quantity); // Calculates the weight of the stack
		}
	}

	return TotalWeight;
}

void UInventoryComponent::DropItem(FName ItemID)
{
	bool bFound = false;
	for (const FInventorySlot& Slot : InventoryContent) // Checks if the item exists
	{
		if (Slot.ItemID == ItemID)
		{
			bFound = true;
			break;
		}
	}

	// Item not found
	if (!bFound)
	{
		UE_LOG(LogTemp, Warning, TEXT("Coudln't find item: %s. Unable to drop!"), *ItemID.ToString());
		return;
	}
	
	FItemData ItemData;
	if (GetItemData(ItemID, ItemData))
	{
		AActor* Owner = GetOwner();
		if (!Owner) // Character not found
		{
			UE_LOG(LogTemp, Warning, TEXT("Character not found, unable to drop item!"));
			return;
		}

		// Item drop position
		FVector Location = Owner->GetActorLocation() + (Owner->GetActorForwardVector() * 100.f);
		FRotator Rotation = Owner->GetActorRotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// Spawns the item
		AItemClass* DroppedActor = GetWorld()->SpawnActor<AItemClass>(ItemData.ItemActorClass, Location, Rotation, SpawnParams);

		if (DroppedActor) // Check if dropped actor was found
		{
			DroppedActor->ItemID = ItemID;
			if (ItemData.ItemMesh) // Checks if it had a mesh to spawn in
				DroppedActor->MyMesh->SetStaticMesh(ItemData.ItemMesh);
			
			UE_LOG(LogTemp, Warning, TEXT("Dropped Item: %s!"), *ItemID.ToString());
			RemoveItem(ItemID, 1); // Removes just 1 item
		}
	}
}

// Sorts the split item arrays
void UInventoryComponent::SortInventory()
{
	int NumItems = InventoryContent.Num();

	// Checks if the array is already sorted (too small)
	if (NumItems < 2)
		return;

	// Controls the size of the subarrays being merged, doubling each time
	for (int32 CurrentSize = 1; CurrentSize <= NumItems - 1; CurrentSize = 2 * CurrentSize)
	{
		// Iterates through the array in chunks to find the start index of the two subarrays to merge
		for (int32 LeftStart = 0; LeftStart < NumItems - 1; LeftStart += 2 * CurrentSize)
		{
			// Calculates the middle and end indices, ensuring we don't go out of bounds for the last chunk
			int32 Mid = FMath::Min(LeftStart + CurrentSize - 1, NumItems - 1);
			int32 RightEnd = FMath::Min(LeftStart + 2 * CurrentSize - 1, NumItems - 1);
			MergeArrays(InventoryContent, LeftStart, Mid, RightEnd);
		}
	}
	OnInventoryUpdated.Broadcast();
}
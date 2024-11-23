// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include <stdexcept>

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	weight = 0.0f;
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UInventoryComponent::AddItem(const FName& row_name)
{
	if(!data_table) return false;
	
	const FItemBase* item = data_table->FindRow<FItemBase>(row_name, TEXT("AddItem"));
	if(item)
	{
		inventory.Add(row_name);
		return true;
	}

	return false;
}

bool UInventoryComponent::RemoveItem(const FName& row_name)
{
	int32 index = inventory.IndexOfByPredicate([&](const FName& item) {
		return item == row_name;
	});
	
	if (index == INDEX_NONE)
	{
		return false; 
	}
	
	inventory.RemoveAt(index);
	
	return true; 
}
	
const FItemBase* UInventoryComponent::GetItemData(const FName& row_name) const
{
	if(data_table)
	{
		return data_table->FindRow<FItemBase>(row_name, TEXT("GetItemData"));
	}

	return nullptr;
}
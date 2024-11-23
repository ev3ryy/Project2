// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Item/Item.h"
#include "../Item/ItemBase.h"
#include "EnvironmentQuery/EnvQueryDebugHelpers.h"
#include "InventoryComponent.generated.h"

class UDataTable;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType )
class PROJECT2_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this component's properties
	UInventoryComponent();
	
	UFUNCTION()
	bool AddItem(const FName& row_name);

	UFUNCTION()
	bool RemoveItem(const FName& row_name);
	
	const FItemBase* GetItemData(const FName& row_name) const;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* data_table;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FName> inventory;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};

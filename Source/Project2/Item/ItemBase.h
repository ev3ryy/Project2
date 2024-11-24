// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemBase.generated.h"

UENUM()
enum class EItemType : uint8
{
	None = 0,
	Cloth,
	Meet
};

USTRUCT(BlueprintType)
struct PROJECT2_API FItemBase : public FTableRowBase
{
	GENERATED_BODY()
	
	FItemBase() :
	item_type(EItemType::None),
	item_name(TEXT("none")),
	item_description((TEXT("simple description"))),
	item_row_name("none"),
	item_weight(0.0f),
	item_stackable(false) {}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemType item_type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString item_name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString item_description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName item_row_name;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float item_weight;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool item_stackable;
};

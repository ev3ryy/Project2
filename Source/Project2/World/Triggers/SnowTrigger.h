// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SnowTrigger.generated.h"

class UBoxComponent;

UCLASS()
class PROJECT2_API ASnowTrigger : public AActor
{
	GENERATED_BODY()

	// backups
	float speed;
	float jump_speed;
	FRotator rotator;
	
public:	
	// Sets default values for this actor's properties
	ASnowTrigger();

	UFUNCTION(BlueprintCallable)
	void BeginOverlap(AActor* OtherActor);

	UFUNCTION(BlueprintCallable)
	void EndOverlap(AActor* OtherActor);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* Zone;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

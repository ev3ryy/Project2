// Copyright Epic Games, Inc. All Rights Reserved.

#include "FP_FirstPersonCharacter.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "../Components/InventoryComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#define COLLISION_WEAPON		ECC_GameTraceChannel1

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AFP_FirstPersonCharacter

AFP_FirstPersonCharacter::AFP_FirstPersonCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0, 0, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	
	inventory_component = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	inventory_component->RegisterComponent();
	
	// Set weapon damage and range
	WeaponRange = 5000.0f;
	WeaponDamage = 25.0f;

	max_health = 100.0f;
	max_armor = 100.0f;

	in_snow = false;
}

void AFP_FirstPersonCharacter::BeginPlay() {
	Super::BeginPlay();

	health = max_health;
	armor = max_armor;
}

void AFP_FirstPersonCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFP_FirstPersonCharacter::OnFire);
	PlayerInputComponent->BindAction("Repair", IE_Pressed, this, &AFP_FirstPersonCharacter::OnRepair);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFP_FirstPersonCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFP_FirstPersonCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis("Turn", this, &AFP_FirstPersonCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AFP_FirstPersonCharacter::LookUpAtRate);
}

void AFP_FirstPersonCharacter::OnFire()
{
	FVector pos;
	FRotator rot;
	Controller->GetPlayerViewPoint(pos, rot);

	FVector end_trace = pos + rot.Vector() * WeaponRange;

	FHitResult hit;
	FCollisionQueryParams cqp;
	cqp.AddIgnoredActor(this);

	if(shoot_sound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), shoot_sound, GetActorLocation(), FRotator::ZeroRotator);	
	}
	
	if (GetWorld()->LineTraceSingleByChannel(hit, pos, end_trace, ECC_Visibility, cqp)) {
		
		UGameplayStatics::ApplyPointDamage(hit.GetActor(), WeaponDamage, rot.Vector(), hit, Controller, this, nullptr);

		if(hit.GetActor() != nullptr) UE_LOG(LogTemp, Log, TEXT("Hitted actor %s on %f damage"), *hit.GetActor()->GetName(), WeaponDamage);
	}
}

void AFP_FirstPersonCharacter::OnRepair()
{
	for(const FName& row_name : inventory_component->inventory)
	{
		const FItemBase* item = inventory_component->GetItemData(row_name);
		if(item && item->item_type == EItemType::Cloth)
		{
			if(armor <= max_armor * 0.8f) // <= 80%
			{
				if(inventory_component->RemoveItem(row_name))
				{
					armor = max_armor;
					UE_LOG(LogTemp, Log, TEXT("Current armor: %f"), armor);
					UE_LOG(LogTemp, Log, TEXT("Remove item: %s"), *row_name.ToString());
					return;
				}
				return;
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("armor doesn`t need repair"));
			}
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("failed to find cloth for repair armor"));
}

void AFP_FirstPersonCharacter::OnPickup(AItem* item)
{
	if(item)
	{
		if(inventory_component->AddItem(item->item_row_name))
		{
			item->Destroy();
			UE_LOG(LogTemp, Log, TEXT("Added item: %s"), *item->item_row_name.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("failed to pickup item: %s"), *item->item_row_name.ToString());
		}
	}
}

void AFP_FirstPersonCharacter::OnPickupRowName(const FName& row_name)
{
	if(!row_name.IsNone())
	{
		if(inventory_component->AddItem(row_name))
		{
			UE_LOG(LogTemp, Log, TEXT("Added item: %s"), *row_name.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("failed to pickup item: %s"), *row_name.ToString());
		}
	}
}

float AFP_FirstPersonCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Log, TEXT("Damage amount: %f"), DamageAmount);

	float percent_of_damage = 0.7f;
	
	if (armor > 0.0f) {
		float damage = DamageAmount * percent_of_damage;
		float health_damage = DamageAmount * (1.0f - percent_of_damage);

		armor = FMath::Clamp(armor - damage, 0.0f, max_armor);
		health = FMath::Clamp(health - health_damage, 0.0f, max_health);
	}
	else {
		health = FMath::Clamp(health - DamageAmount, 0.0f, max_health);
	}

	UE_LOG(LogTemp, Log, TEXT("Health: %f, Armor: %f"), health, armor);

	// death check
	if (health <= 0.0f) {
		UE_LOG(LogTemp, Log, TEXT("%s is dead!"), *this->GetName());
	
		this->Destroy();
	}

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AFP_FirstPersonCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// Add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AFP_FirstPersonCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// Add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFP_FirstPersonCharacter::TurnAtRate(float Rate)
{
	if(in_snow)
	{
		Rate *= 0.5f;
	}
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFP_FirstPersonCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

#include "SnowTrigger.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Project2/FP_FirstPerson/FP_FirstPersonCharacter.h"

ASnowTrigger::ASnowTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	Zone = CreateDefaultSubobject<UBoxComponent>(TEXT("Zone Collider"));
	RootComponent = Zone;
}

// Called when the game starts or when spawned
void ASnowTrigger::BeginPlay()
{
	Super::BeginPlay();
}
void ASnowTrigger::BeginOverlap(AActor* OtherActor)
{
	auto pawn = Cast<ACharacter>(OtherActor);
	
	if(pawn)
	{
		speed = pawn->GetCharacterMovement()->MaxWalkSpeed;
		jump_speed = pawn->GetCharacterMovement()->JumpZVelocity;
		rotator = pawn->GetCharacterMovement()->RotationRate;
		
		pawn->GetCharacterMovement()->MaxWalkSpeed = speed / 2;
		pawn->GetCharacterMovement()->JumpZVelocity = jump_speed / 2.5f;
		pawn->GetCharacterMovement()->RotationRate = FRotator(0, rotator.Yaw / 2, 0);

		auto character = Cast<AFP_FirstPersonCharacter>(pawn);

		if(character)
		{
			character->in_snow = true;
		}
	}
}

void ASnowTrigger::EndOverlap(AActor* OtherActor)
{
	auto pawn = Cast<ACharacter>(OtherActor);
	
	if(pawn)
	{
		pawn->GetCharacterMovement()->MaxWalkSpeed = speed;
		pawn->GetCharacterMovement()->JumpZVelocity = jump_speed;
		pawn->GetCharacterMovement()->RotationRate = rotator;

		auto character = Cast<AFP_FirstPersonCharacter>(pawn);

		if(character)
		{
			character->in_snow = false;
		}
	}
}

// Called every frame
void ASnowTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


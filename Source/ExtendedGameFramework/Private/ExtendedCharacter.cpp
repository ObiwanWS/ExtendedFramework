// Copyright (c) 2024 Obiwan Medeiros.

#include "ExtendedCharacter.h"

#include "ExtendedCharacterMovement.h"

#include <Net/Core/PushModel/PushModel.h>
#include <Net/UnrealNetwork.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(ExtendedCharacter)

AExtendedCharacter::AExtendedCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UExtendedCharacterMovement>(ACharacter::CharacterMovementComponentName))
{
	// Avoid ticking characters if possible.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// Store the ExtendedCharacterMovement.
	ExtendedCharacterMovementComponent = GetCharacterMovement<UExtendedCharacterMovement>();

	// Bind the location correction delegate.
	ExtendedCharacterMovementComponent->OnClientLocationCorrected.BindUObject(this, &AExtendedCharacter::OnClientLocationCorrected);

	// Sets default values for this character.
	bReplicateAcceleration = false;
	ReplicatedAcceleration = FReplicatedAcceleration();
}

void AExtendedCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams sharedParams;
	sharedParams.bIsPushBased = true;
	sharedParams.Condition = COND_SimulatedOnly;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ReplicatedAcceleration, sharedParams);
}

void AExtendedCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	if (bReplicateAcceleration)
	{
		// Compress Acceleration: XY components as direction + magnitude, Z component as direct value.
		const float maxAccel = ExtendedCharacterMovementComponent->MaxAcceleration;
		const FVector currentAccel = ExtendedCharacterMovementComponent->GetCurrentAcceleration();
		double accelXYRadians, accelXYMagnitude;
		FMath::CartesianToPolar(currentAccel.X, currentAccel.Y, accelXYMagnitude, accelXYRadians);

		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ReplicatedAcceleration, this);
		// int64 to int8 is unsafe type cast and could trigger warnings/errors depending on build settings.
		ReplicatedAcceleration.AccelXYRadians = FMath::FloorToInt((accelXYRadians / TWO_PI) * 255.0); // [0, 2PI] -> [0, 255]
		ReplicatedAcceleration.AccelXYMagnitude = FMath::FloorToInt((accelXYMagnitude / maxAccel) * 255.0);	// [0, MaxAccel] -> [0, 255]
		ReplicatedAcceleration.AccelZ = FMath::FloorToInt((currentAccel.Z / maxAccel) * 127.0); // [-MaxAccel, MaxAccel] -> [-127, 127]
	}
}

UExtendedCharacterMovement* AExtendedCharacter::GetExtendedCharacterMovementComponent()
{
	return ExtendedCharacterMovementComponent;
}

void AExtendedCharacter::OnClientLocationCorrected()
{
	// Set timer for next tick since in this tick the client has not been corrected yet.
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AExtendedCharacter::CheckImpossibleJumpState);
}

void AExtendedCharacter::CheckImpossibleJumpState()
{
	// This is a fix for a bug where the character gets corrected to the floor after a jump and the jump state isn't properly handled on the client, making so the client can't jump again.
	if ((GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking || GetCharacterMovement()->MovementMode == EMovementMode::MOVE_NavWalking) && JumpCurrentCount > 0)
	{
		ResetJumpState();
	}
}

void AExtendedCharacter::OnRep_ReplicatedAcceleration()
{
	// Decompress Acceleration.
	const double accelXYMagnitude = double(ReplicatedAcceleration.AccelXYMagnitude) * ExtendedCharacterMovementComponent->MaxAcceleration / 255.0; // [0, 255] -> [0, MaxAccel]
	const double accelXYRadians = double(ReplicatedAcceleration.AccelXYRadians) * TWO_PI / 255.0; // [0, 255] -> [0, 2PI]

	FVector unpackedAcceleration(FVector::ZeroVector);
	FMath::PolarToCartesian(accelXYMagnitude, accelXYRadians, unpackedAcceleration.X, unpackedAcceleration.Y);
	unpackedAcceleration.Z = ReplicatedAcceleration.AccelZ * ExtendedCharacterMovementComponent->MaxAcceleration / 127.0; // [-127, 127] -> [-MaxAccel, MaxAccel]

	ExtendedCharacterMovementComponent->SetReplicatedAcceleration(unpackedAcceleration);
}

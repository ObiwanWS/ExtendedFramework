// Copyright (c) 2024 Obiwan Medeiros.

#pragma once

#include <GameFramework/Character.h>

#include "ExtendedCharacter.generated.h"

class UExtendedCharacterMovement;

/** Compressed representation of acceleration. */
USTRUCT()
struct FReplicatedAcceleration
{
	GENERATED_BODY()

	/** Direction of XY accel component, quantized to represent[0, 2 * pi]. */
	UPROPERTY()
	uint8 AccelXYRadians;

	/** Accel rate of XY component, quantized to represent [0, MaxAcceleration]. */
	UPROPERTY()
	uint8 AccelXYMagnitude;

	/** Raw Z accel rate component, quantized to represent [-MaxAcceleration, MaxAcceleration]. */
	UPROPERTY()
	int8 AccelZ;

	FReplicatedAcceleration()
	{
		AccelXYRadians = 0U;
		AccelXYMagnitude = 0U;
		AccelZ = 0U;
	}
};

/** Extended character pawn class. */
UCLASS(MinimalAPI)
class AExtendedCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	/** Whether to replicate acceleration to simulated proxies. */
	UPROPERTY(Category = "Character", EditAnywhere, BlueprintReadOnly)
	uint8 bReplicateAcceleration : 1;

	/** Legacy constructor used to allow overrides of base classes. */
	AExtendedCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~Begin of UObject implementation.
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~End of UObject implementation.

	//~Begin of AActor implementation.
	void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
	//~End of AActor implementation.

	/** Returns the Extended Character Movement Component used by this character. */
	UFUNCTION(BlueprintCallable, Category = "ExtendedFramework|Character")
	EXTENDEDGAMEFRAMEWORK_API UExtendedCharacterMovement* GetExtendedCharacterMovementComponent();

protected:
	/** Compressed acceleration that will be replicated on PreReplication. */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_ReplicatedAcceleration)
	FReplicatedAcceleration ReplicatedAcceleration;

	/** Called before the character gets corrected on the client by the server. */
	void OnClientLocationCorrected();

	/** This will check if the character is in movement mode MODE_Walking and has JumpCurrentCount > 0, which should never happen, then fix it. */
	void CheckImpossibleJumpState();

private:
	/** ExtendedCharacterMovement component used in this character. */
	TObjectPtr<UExtendedCharacterMovement> ExtendedCharacterMovementComponent;

	/** Decompresses the acceleration on simulated proxies and sends the data to the Character Movement Component. */
	UFUNCTION()
	void OnRep_ReplicatedAcceleration();
};

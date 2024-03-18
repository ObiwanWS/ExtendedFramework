// Copyright (c) 2024 Obiwan Medeiros.

#pragma once

#include <GameFramework/CharacterMovementComponent.h>

#include "ExtendedCharacterMovement.generated.h"

/** Delegate that will be called when the client's location gets corrected. */
DECLARE_DELEGATE(FOnClientLocationCorrected);

UCLASS(MinimalAPI)
class UExtendedCharacterMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	/** Called on the client before the character that owns this movement component has it's location corrected by the server. */
	FOnClientLocationCorrected OnClientLocationCorrected;

	/** Sets default values for this movement component. */
	UExtendedCharacterMovement();

	/** */
	void SetReplicatedAcceleration(const FVector& InAcceleration);

protected:
	//~Begin of UCharacterMovementComponent implementation.
	void SimulateMovement(float DeltaTime) override;
	void OnClientCorrectionReceived(class FNetworkPredictionData_Client_Character& ClientData, float TimeStamp, FVector NewLocation, FVector NewVelocity, UPrimitiveComponent* NewBase, FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode, FVector ServerGravityDirection) override;
	//~End of UCharacterMovementComponent implementation.

private:
	/** */
	UPROPERTY(Transient)
	uint8 bHasReplicatedAcceleration : 1;
};

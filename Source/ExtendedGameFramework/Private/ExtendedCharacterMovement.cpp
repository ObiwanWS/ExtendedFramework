// Copyright (c) 2024 Obiwan Medeiros.

#include "ExtendedCharacterMovement.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ExtendedCharacterMovement)

UExtendedCharacterMovement::UExtendedCharacterMovement()
{
	// Enable some optimization settings by default.
	bEnableServerDualMoveScopedMovementUpdates = true;
	bNetworkSkipProxyPredictionOnNetUpdate = true;

	// Use acceleration instead of velocity for paths. 
	// This allows animations that use acceleration to display properly for bots.
	bUseAccelerationForPaths = true;

	// Sets default values for this component.
	bHasReplicatedAcceleration = false;
}

void UExtendedCharacterMovement::SetReplicatedAcceleration(const FVector& InAcceleration)
{
	bHasReplicatedAcceleration = true;
	Acceleration = InAcceleration;
}

void UExtendedCharacterMovement::SimulateMovement(float DeltaTime)
{
	if (bHasReplicatedAcceleration)
	{
		// Preserve our replicated acceleration.
		const FVector originalAcceleration = Acceleration;
		Super::SimulateMovement(DeltaTime);
		Acceleration = originalAcceleration;
	}
	else
	{
		Super::SimulateMovement(DeltaTime);
	}
}

void UExtendedCharacterMovement::OnClientCorrectionReceived(class FNetworkPredictionData_Client_Character& ClientData, float TimeStamp, FVector NewLocation, FVector NewVelocity, UPrimitiveComponent* NewBase, FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode, FVector ServerGravityDirection)
{
	// Check the delta of the current location and the previous location to know if the correction changed our location.
	const FVector locationDelta = (ClientData.LastAckedMove.IsValid() ? ClientData.LastAckedMove->SavedLocation : UpdatedComponent->GetComponentLocation()) - NewLocation;
	if (!locationDelta.IsNearlyZero(0.01))
	{
		OnClientLocationCorrected.ExecuteIfBound();
	}

	// Super of this function only handles debugging, on shipping builds we won't call super since debugging will be removed anyway.
#if !UE_BUILD_SHIPPING
	Super::OnClientCorrectionReceived(ClientData, TimeStamp, NewLocation, NewVelocity, NewBase, NewBaseBoneName, bHasBase, bBaseRelativePosition, ServerMovementMode, ServerGravityDirection);
#endif // !UE_BUILD_SHIPPING
}

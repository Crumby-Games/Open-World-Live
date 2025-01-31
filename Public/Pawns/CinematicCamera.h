#pragma once // Prevents repeated compilation of the same file

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CinematicCamera.generated.h"

UCLASS()
class SLASH_API ACinematicCamera : public APawn
{
	GENERATED_BODY()

public:
	ACinematicCamera();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	// Movement controls
	void MoveForward(float Value); 	// Local x axis
	void Strafe(float Value); 		// Local y axis
	void Fly(float Value); 			// Z axis
	void Turn(float Value); 		// Local yaw
	void LookUp(float Value); 		// Local pitch
	void SpeedUp(float Value); 		// Movement speed

	// Forward declaration for components
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* SphereCollider;

	UPROPERTY(VisibleAnywhere)
	class UFloatingPawnMovement* MovementComponent;

private:
	// Camera smoothing configuration.
	// All are accessible within the event graph but not changable since it is not needed with current implementation. Same applies for properties per instance.

	// Maximum length of AngularVelocity vector
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	float MaxAngularSpeed = 1.f;

	// Acceleration factor.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	float AngularAccelerationCoefficient = 0.05f; // Not representative of acceleration as a whole since this is scaled by the speed of mouse input or degree of controller input

	// Deceleration factor
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	float AngularDeceleration = 1.f;

	// Whether or not rotation is simultaneously decelerated while being accelerated
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool bAlwaysApplyDeceleration = false;

	// Whether there there is input to keep rotating
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool bRotationInputPresent = false;

	// Stores velocity about each axis as a value corresponding to that axis. This is because FRotator would behave unexpectedly storing high velocities because of Euler angles.
	FVector AngularVelocity = FVector(0);
}; 

#include "Pawns/CinematicCamera.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"


ACinematicCamera::ACinematicCamera()
{
	// Allow ACinematicCamera::Tick()
	PrimaryActorTick.bCanEverTick = true;

	// Collider
	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	RootComponent = SphereCollider;

	// Camera component
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);
	
	// Movement component
	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>("MovementComponent");
	
}

void ACinematicCamera::BeginPlay()
{
	Super::BeginPlay();
}

void ACinematicCamera::Tick(float DeltaTime)
{
	// Reset whether a rotation input has been entered this tick
	bRotationInputPresent = false;

	// If rotating faster than max speed, clamp velocity to max speed.
	if (AngularVelocity.Length() > MaxAngularSpeed)
	{
		AngularVelocity = AngularVelocity.GetSafeNormal() * MaxAngularSpeed;
	}

	// Only decelerate rotation if either not accelerating or always decelerating
	if (bAlwaysApplyDeceleration || !bRotationInputPresent)
	{
		// Unless there is very little velocity, decrease by fixed amount. Otherwise, reset to zero vector.
		if (AngularVelocity.Length() >= AngularDeceleration * DeltaTime)
		{
			AngularVelocity -= AngularVelocity.GetSafeNormal() * AngularDeceleration * DeltaTime;
		}
		else
		{
			AngularVelocity = FVector(0);
		}
	}

	// Rotate about individual axes based on rotational velocity. (Multiplying by a hundred is an arbitrary step that makes the numbers nicer to work with in editor)
	AddActorLocalRotation(FRotator(0, AngularVelocity.Z * DeltaTime * 100.f, 0));		 // Yaw rotation. On actor since it should affect forward direction
	Camera->AddLocalRotation(FRotator(AngularVelocity.Y * DeltaTime * 100.f, 0, 0)); // Pitch rotation. On camera component since it should affect viewport but not forward direction (buttons are used to control Z axis)
}

// Movement on local X axis
void ACinematicCamera::MoveForward(float Value)
{
	// Only do behaviour if controller valid and input present
	if (Controller && Value != 0.f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

// Movement on local Y axis
void ACinematicCamera::Strafe(float Value)
{
	// Only do behaviour if controller valid and input present
	if (Controller && Value != 0.f)
	{
		AddMovementInput(-FVector::CrossProduct(GetActorForwardVector(), FVector(0, 0, 1).GetSafeNormal()), Value);
	}
}

// Movement on Z axis (same local and global since roll and pitch of actor is unchanged)
void ACinematicCamera::Fly(float Value)
{
	// Only do behaviour if controller valid and input present
	if (Controller && Value != 0.f)
	{
		AddMovementInput(FVector(0, 0, 1), Value);
	}
}

// Change in yaw
void ACinematicCamera::Turn(float Value)
{
	// Only do behaviour if input present
	if (Value != 0.f)
	{
		bRotationInputPresent = true;
		AngularVelocity.Z += Value * AngularAccelerationCoefficient;
	}
}

// Change in pitch
void ACinematicCamera::LookUp(float Value)
{
	// Only do behaviour input present
	if (Value != 0.f)
	{
		bRotationInputPresent = true;
		AngularVelocity.Y += Value * AngularAccelerationCoefficient;
	}
}

// Change movement speed
void ACinematicCamera::SpeedUp(float Value)
{
	// Only do behaviour if component valid and input present
	if (MovementComponent && Value != 0.f)
	{
		// Exponential calculation so that it doesn't take long to traverse long distance and then slow down for close-ups
		MovementComponent->MaxSpeed = FMath::Max(40.f,MovementComponent->MaxSpeed * (1 + Value / 20));
	}
			
}

void ACinematicCamera::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Binds movement inputs to movement methods
	// (Deprecated UE5 behaviour. Will be changed to use the Enhanced Input system.)
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACinematicCamera::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ACinematicCamera::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ACinematicCamera::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Strafe"), this, &ACinematicCamera::Strafe);
	PlayerInputComponent->BindAxis(TEXT("Fly"), this, &ACinematicCamera::Fly);
	PlayerInputComponent->BindAxis(TEXT("SpeedUp"), this, &ACinematicCamera::SpeedUp);
}
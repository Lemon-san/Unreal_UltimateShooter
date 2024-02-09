// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ShooterAnimInstance.h"
#include "Character/ShooterCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Weapons/Weapon.h"

void UShooterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	if (!ShooterCharacter) return;

	ShooterMovement = ShooterCharacter->GetCharacterMovement();

}

void UShooterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (!ShooterMovement) return;

	GroundSpeed = UKismetMathLibrary::VSizeXY(ShooterMovement->Velocity);
	isFalling = ShooterMovement->IsFalling();

	if (ShooterMovement->GetCurrentAcceleration().Size() > 0.f)
	{
		isAccelerating = true;
	}

	else
	{
		isAccelerating = false;
	}

	bCrouching = ShooterCharacter->GetCrouching();
	bReloading = ShooterCharacter->GetCombatState() == ECombatState::ECS_Reloading;
	bEquipping = ShooterCharacter->GetCombatState() == ECombatState::ECS_Equipping;
	bShouldUseFabrik = ShooterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied ||
		ShooterCharacter->GetCombatState() == ECombatState::ECS_FireTimerInProgress;

	if (bReloading == true)
	{
		ZoomState = EZoomState::EZS_Reloading;
	}
	else if (isFalling)
	{
		ZoomState = EZoomState::EZS_InAir;
	}
	else
	{
		ZoomState = EZoomState::EZS_DefaultAim;
	}

	if (ShooterCharacter->GetEquippedWeapon())
	{
		EquippedWeaponType = ShooterCharacter->GetEquippedWeapon()->GetWeaponType();
	}

	FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());
	MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
	

	if (ShooterCharacter->GetVelocity().Size() > 0.f)
	{
		LastMovementOffsetYaw = MovementOffsetYaw;
	}

	TurnInPlace();
	Lean(DeltaTime);
}

void UShooterAnimInstance::TurnInPlace()
{
	if (!ShooterCharacter) return;

	Pitch = ShooterCharacter->GetBaseAimRotation().Pitch;


	if (GroundSpeed > 0 || isFalling)
	{
		//TODO: Don't turn in place while moving
		RootYawOffset = 0.f;
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		TIPCharacterYawLastFrame = TIPCharacterYaw;

		RotationCurveLastFrame = 0.f;
		RotationCurve = 0.f;
	}
	else
	{
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;

		const float TIPYawDelta = TIPCharacterYaw - TIPCharacterYawLastFrame;

		//Clamps Yaw offset to -180 and 180
		RootYawOffset  = UKismetMathLibrary::NormalizeAxis(RootYawOffset - TIPYawDelta);

		const float Turning = GetCurveValue(TEXT("Turning"));
		if (Turning > 0)
		{
			bTurningInPlace = true;

			RotationCurveLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));

			const float DeltaRotation = RotationCurve - RotationCurveLastFrame;

			//RootYawOffset > 0, turning left. < 0 turning right.
			RootYawOffset > 0 ? RootYawOffset -= DeltaRotation : RootYawOffset += DeltaRotation;

			const float ABSRootYawOffset = FMath::Abs(RootYawOffset);

			if (ABSRootYawOffset > 90.f)
			{
				const float YawExcess = ABSRootYawOffset - 90.f;
				RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
			}
		}

		else
		{
			bTurningInPlace = false;
		}	
		
	}

	//sets the recoil rate
	if (bTurningInPlace)
	{
		(bReloading || bEquipping) ? WeaponRecoilRate = 1.f : WeaponRecoilRate = 0.f;
	}
	else
	{
		if (bCrouching)
		{
			(bReloading || bEquipping) ? WeaponRecoilRate = 1.f : WeaponRecoilRate = 0.1f;
		}

		else
		{
			(ZoomState == EZoomState::EZS_ZoomAim || bReloading || bEquipping) ? WeaponRecoilRate = 1.f : WeaponRecoilRate = 0.5f;
		}
	}
}

void UShooterAnimInstance::Lean(float DeltaTime)
{
	if (!ShooterCharacter) return;

	LeanCharacterYawLastFrame = LeanCharacterYaw;
	LeanCharacterYaw = ShooterCharacter->GetActorRotation();

	FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(LeanCharacterYaw, LeanCharacterYawLastFrame);

	const float Target = (Delta.Yaw) / DeltaTime;
	const float Interp = FMath::FInterpTo(YawDelta, Target, DeltaTime, 6.f);

	YawDelta = FMath::Clamp(Interp, -90.f, 90.f);

	

}


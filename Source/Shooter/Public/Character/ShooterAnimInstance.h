// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"
#include "ShooterAnimInstance.generated.h"

class AShooterCharacter;
class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class SHOOTER_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

protected:

	void TurnInPlace();
	void Lean(float DeltaTime);

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	UCharacterMovementComponent* ShooterMovement;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	AShooterCharacter* ShooterCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float GroundSpeed = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool isFalling = false;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool isAccelerating = false;

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	float MovementOffsetYaw = 0.f;

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	float LastMovementOffsetYaw = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	EZoomState ZoomState = EZoomState::EZS_DefaultAim;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	EWeaponType EquippedWeaponType = EWeaponType::EWT_MAX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bReloading = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float Pitch = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float TIPCharacterYaw = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float TIPCharacterYawLastFrame = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	FRotator LeanCharacterYaw = FRotator(0.f);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	FRotator LeanCharacterYawLastFrame = FRotator(0.f);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float RootYawOffset = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float YawDelta = 0;

	float RotationCurve = 0;

	float RotationCurveLastFrame = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	float WeaponRecoilRate = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bTurningInPlace = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bCrouching = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bEquipping;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bShouldUseFabrik = false;

};

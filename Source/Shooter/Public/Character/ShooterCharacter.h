// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "CharacterTypes.h"
#include "ShooterCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class AWeapon;
class AItem;
class AAmmo;
class UParticleSystem;

USTRUCT(BlueprintType)
struct FInterpLocation
{
	GENERATED_BODY()

	//Scene Component to use for its location for interping
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneComponent;

	//Number of items interping to/at this scene component location
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHighlightIconDelegate, int32, SlotIndex, bool, bStartAnimation);

UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AShooterCharacter();
	virtual void Tick(float DeltaTime) override;
	
	void SetCameraFieldOfView(float DeltaTime);
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void HighlightInventorySlot();
	void UnHighlightInventorySlot();

	void Stun();

protected:

	virtual void BeginPlay() override;
	virtual void Jump() override;
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void SetLookRate();
	void FireWeapon();
	void SendBullet();
	bool GetBeamEndLocation(FHitResult& BeamEndResult, const FVector& SocketTransform);
	void SpawnImpactParticles(const FHitResult& BeamEndPoint);
	bool WeaponLineTrace(const FVector& SocketTransform,FHitResult& BeamEndResult, FVector& BeamEndPoint);
	void GetViewPortSize(FVector2D& ViewportSize);
	bool isScreenToWorld(FVector& CrosshairWorldPosition, FVector& CrosshairWorldDirection);
	void SpawnBeamParticles(const FTransform& SocketTransform, const FHitResult& BeamEndPoint);
	void PlayMontage(UAnimMontage* Montage, const FName& SectionName);
	void PlayGunshotSound();
	void SpawnMuzzleParticles(const USkeletalMeshSocket* Socket);
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);
	AWeapon* SpawnDefaultWeapon();
	void EquipWeapon(AWeapon* WeaponToEquip, bool bSwapping = false);
	void DropWeapon();
	void SwapWeapon(AWeapon* WeaponToSwap);
	void PickupAmmo(AAmmo* Ammo);
	bool WeaponHasAmmo();
	void InitializeAmmoMap();
	bool CarryingAmmo();

	void InterpCapsuleHalfHeight(float DeltaTime);
	void InitializeInterpLocations();
	
	int32 GetEmptyInventorySlot();

	void Die();
	bool bDying = false;

	UFUNCTION(BlueprintCallable)
	void FinishDeath();

	UFUNCTION(BlueprintCallable)
	void EndStun();

	UFUNCTION(BlueprintCallable)
	EPhysicalSurface GetSurfaceType();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CurrentCapsuleHalfHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StandingCapsuleHalfHeight = 88.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CrouchingCapsuleHalfHeight = 54.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	USceneComponent* WeaponInterpComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	USceneComponent* ItemInterpComp1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	USceneComponent* ItemInterpComp2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	USceneComponent* ItemInterpComp3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	USceneComponent* ItemInterpComp4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	USceneComponent* ItemInterpComp5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	USceneComponent* ItemInterpComp6;
	

private:

	void AimingButtonPressed();
	void AimingButtonReleased();
	void Aim();
	void StopAiming();
	void SelectButtonPressed();
	void SelectButtonReleased();
	void FireButtonPressed();
	void FireButtonReleased();
	void ReloadButtonPressed();
	void ReloadWeapon();
	void CrouchButtonPressed();
	void SwitchButtonPressed(const FInputActionValue& Value);
	void ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex);

	void CalculateCrosshairSpread(float DeltaTime);
	bool isZooming();
	bool isBulletFiring();
	bool isNotBulletFiring();
	void TraceForItems();
	void StartCrosshairBulletFire();
	void StartFireTimer();
	void ResetPickupSoundTimer();
	void ResetEquipSoundTimer();

	UPROPERTY(EditAnywhere, Category = "Item")
	float PickupSoundResetTime = 0.2f;
	UPROPERTY(EditAnywhere, Category = "Item")
	float EquipSoundResetTime = 0.2f;
	
	float BaseGroundFriction = 2.f;
	float CrouchingGroundFriction = 100.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraDefaultFOV;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraZoomedFOV;

	float CameraCurrentFOV;
	bool bShouldTraceForItems;
	bool bShouldFire = true;
	bool bShouldPlayPickupSound = true;
	bool bShouldPlayEquipSound = true;

	int8 OverlappedItemCount;

	FTimerHandle CrosshairShootTimer;
	FTimerHandle AutoFireTimer;
	FTimerHandle PickupSoundTimer;
	FTimerHandle EquipSoundTimer;

	UFUNCTION()
	void FinishCrosshairBulletFire();

	UFUNCTION()
	void AutoFireReset();

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	UFUNCTION(BlueprintCallable)
	void GrabClip();

	UFUNCTION (BlueprintCallable)
	void ReleaseClip();

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputMappingContext* CharacterMappingContext;

	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputAction* ShootAction;

	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputAction* SelectAction;

	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputAction* ReloadAction;

	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, Category = "Inputs")
	UInputAction* SwitchAction;

	UPROPERTY(EditAnywhere, Category = "Particles")
	UParticleSystem* WeaponImpactParticles;

	UPROPERTY(EditAnywhere, Category = "Particles")
	UParticleSystem* BeamParticles;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* HipFireMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* EquippingMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float GunLineTraceLength = 50000.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomInterpSpeed;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	float CrosshairSpreadMultiplier;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	float CrosshairVelocityFactor;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	float CrosshairInAirFactor;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	float CrosshairAimFactor;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	float CrosshairShootingFactor;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	float ShootTimeDuration;

	UPROPERTY(EditAnywhere, Category = "Combat", meta = (ClampMin = "0.0", ClampMax = "0.0", UIMin = "0.0", UIMax = "1.0"))
	float BaseLookRate;

	UPROPERTY(EditAnywhere, Category = "Combat", meta = (ClampMin = "0.0", ClampMax = "0.0", UIMin = "0.0", UIMax = "1.0"))
	float HipLookRate;

	UPROPERTY(EditAnywhere, Category = "Combat", meta = (ClampMin = "0.0", ClampMax = "0.0", UIMin = "0.0", UIMax = "1.0"))
	float AimingLookRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EZoomState ZoomState = EZoomState::EZS_DefaultAim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EBulletState BulletState = EBulletState::EBS_NoFire;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UPROPERTY(VisibleAnywhere, Category = "Items")
	AItem* TraceHitItemLastFrame;

	UPROPERTY(VisibleAnywhere, Category = "Items")
	AItem* TraceHitItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AWeapon> DefaultWeaponClass;

	UPROPERTY(EditAnywhere, Category = "Item")
	float CameraInterpDistance = 125.f;

	UPROPERTY(EditAnywhere, Category = "Item")
	float CameraInterpElevation = 65.f;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;

	UPROPERTY(EditAnywhere, Category = "Item")
	int32 Starting9mmAmmo;

	UPROPERTY(EditAnywhere, Category = "Item")
	int32 StartingARAmmo;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	FTransform ClipTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	USceneComponent* HandSceneComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bCrouching = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float BaseMovementSpeed = 650.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float CrouchMovementSpeed = 300.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bAimingButtonPressed = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TArray<FInterpLocation> InterpLocations;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<AItem*> Inventory;

	//Delegate for sending slot information to InventoryBar when equipping
	UPROPERTY(BlueprintAssignable, Category = "Delegate", meta = (AllowPrivateAccess = "true"))
	FEquipItemDelegate EquipItemDelegate;

	//Delegate for sending slot information for playing icon animation
	UPROPERTY(BlueprintAssignable, Category = "Delegate", meta = (AllowPrivateAccess = "true"))
	FHighlightIconDelegate HighlightIconDelegate;

	//The index for the currently highlighted slot
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	int32 HighlightedSlot = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	float Health = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties", meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 100.f;

	const int32 INVENTORY_CAPACITY = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	USoundBase* ImpactSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BloodParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float StunChance = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Montages", meta = (AllowPrivateAccess = "true"))
	TArray<FName> MontageDeaths;

public:

	FORCEINLINE EZoomState GetZoomState() const { return ZoomState; };
	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; };
	FORCEINLINE UCameraComponent* GetCamera() const{ return Camera; };
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; };
	FORCEINLINE bool GetCrouching() const { return bCrouching; };
	FORCEINLINE bool GetShouldPlayPickupSound() const { return bShouldPlayPickupSound; };
	FORCEINLINE bool GetShouldPlayEquipSound() const { return bShouldPlayEquipSound; };
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; };
	FORCEINLINE USoundBase* GetImpactSound() const { return ImpactSound; };
	FORCEINLINE UParticleSystem* GetBloodParticles() const { return BloodParticles; };
	FORCEINLINE float GetStunChance() const { return StunChance; };


	FInterpLocation GetInterpLocation(int32 Index);

	//no longer needed aitem has interp location
	//FVector GetCameraInterpLocation();
	int32 GetInterpLocationIndex();

	void IncrementOverlappedItemCount(int8 Amount);
	void IncrementInterpLocItemCount(int32 Index, int32 Amount);

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

	void GetPickupItem(AItem* Item);

	void StartPickupSoundTimer();
	void StartEquipSoundTimer();


};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Character/CharacterTypes.h"
#include "Engine/DataTable.h"
#include "Weapon.generated.h"

/**
 * 
 */

class USoundBase;
class UWidgetComponent;

USTRUCT(BlueprintType)
struct FWeaponDataTable : public FTableRowBase
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeaponAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MagazineCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* PickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* EquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* InventoryIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* AmmoIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* MaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaterialIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ClipBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ReloadMontageSection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> AnimBP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairMiddle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairBottom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairTop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AutoFireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BoneToHide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAutomatic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HeadShotDamage;


};
UCLASS()
class SHOOTER_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;

	void ThrowWeapon();
	void DecrementAmmo();
	void ReloadAmmo(int32 Amount);
	bool ClipIsFull();

	void StartSlideTimer();

private:
	AWeapon();

	int32 PreviousMaterialIndex;

protected:
	void StopFalling();
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	int32 Ammo = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	int32 MagazineCapacity = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	EWeaponType WeaponType = EWeaponType::EWT_SubmachineGun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	EAmmoType AmmoType = EAmmoType::EAT_9mm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	float HeadShotDamage;

	

private:
	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime;
	bool bFalling;

	FTimerHandle SlideTimer;


	void FinishMovingSlide();
	void UpdateSlideDisplacement();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bMovingClip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FName ReloadMontageSection = TEXT("ReloadSMG");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FName ClipBoneName = TEXT("smg_clip");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Data Table", meta = (AllowPrivateAccess = "true"))
	UDataTable* WeaponDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Data Table", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrosshairMiddle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Data Table", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrosshairRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Data Table", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrosshairLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Data Table", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrosshairBottom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Data Table", meta = (AllowPrivateAccess = "true"))
	UTexture2D* CrosshairTop;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Data Table", meta = (AllowPrivateAccess = "true"))
	float AutoFireRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Data Table", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* MuzzleFlash;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Data Table", meta = (AllowPrivateAccess = "true"))
	USoundBase* FireSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Data Table", meta = (AllowPrivateAccess = "true"))
	FName BoneToHide;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	float SlideDisplacement = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* SlideDisplacementCurve;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	bool bMovingSlide = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	float MaxSlideDisplacement = 4.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	float SlideDisplacementTime = 0.2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	float MaxRecoilRotation = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	float RecoilRotation = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pistol", meta = (AllowPrivateAccess = "true"))
	bool bAutomatic = true;

public:
	
	FORCEINLINE int32  GetAmmo() const { return Ammo; };
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; };
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; };
	FORCEINLINE FName GetReloadMontageSection() const { return ReloadMontageSection; };
	FORCEINLINE int32 GetMagazineCapacity() const { return MagazineCapacity; };
	FORCEINLINE FName GetClipBoneName() const { return ClipBoneName; };
	FORCEINLINE void SetMovingClip(bool Move) { bMovingClip = Move; };
	FORCEINLINE float GetAutoFireRate() const { return AutoFireRate; };
	FORCEINLINE UParticleSystem* GetMuzzleFlash() const { return MuzzleFlash; };
	FORCEINLINE USoundBase* GetFireSound() const { return FireSound; };
	FORCEINLINE bool GetAutomatic() const { return bAutomatic; };
	FORCEINLINE float GetDamage() const { return Damage; };
	FORCEINLINE float GetHeadShotDamage() const { return HeadShotDamage; };
};

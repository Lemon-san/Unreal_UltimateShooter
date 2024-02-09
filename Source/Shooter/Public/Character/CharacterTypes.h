#pragma once

UENUM(BlueprintType)
enum class EZoomState : uint8
{
	EZS_DefaultAim UMETA(DisplayName = "DefaultAim"),
	EZS_ZoomAim UMETA(DisplayName = "ZoomAim"),
	EZS_Reloading UMETA(DisplayName = "Reloading"),
	EZS_InAir UMETA(DisplayName ="InAir"),
	EZS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EBulletState : uint8
{
	EBS_NoFire UMETA(DisplayName = "NoFire"),
	EBS_Fire UMETA(DisplayName = "Fire"),
	EBS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EBulletRarity : uint8
{
	EBR_Damaged UMETA(DisplayName = "Damaged"),
	EBR_Common UMETA(DisplayName = "Common"),
	EBR_Uncommon UMETA(DisplayName = "Uncommon"),
	EBR_Rare UMETA(DisplayName = "Rare"),
	EBR_Legendary UMETA(DisplayName = "Legendary"),
	EBR_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Pickup UMETA(DisplayName = "Pickup"),
	EIS_EquippedInterping UMETA(DisplayName = "EquipInterping"),
	EIS_Pickedup UMETA(DisplayName = "Pickedup"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_Falling UMETA(DisplayName = "Falling"),
	EIS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_9mm UMETA(DisplayName = "9mm"),
	EAT_AR UMETA(DisplayName = "AssaultRifle"),
	EAT_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_FireTimerInProgress UMETA(DisplayName = "FireTimeInProgress"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_Equipping UMETA(DisplayName = "Equipping"),
	ECS_Stunned UMETA(DisplayName = "Stunned"),
	ECS_MAX UMETA(DisplayName = "DefaultMAX")
};


UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_SubmachineGun UMETA(DisplayName = "SubmachineGun"),
	EWT_AssaultRifle UMETA(DisplayName = "AssaultRifle"),
	EWT_Pistol UMETA(DisplayName = "Pistol"),
	EWT_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Weapon UMETA(DisplayName = "Weapon"),
	EIT_Ammo UMETA(DisplayName = "Ammo"),
	EIT_MAX UMETA(DisplayName = "DefaultMAX")
};
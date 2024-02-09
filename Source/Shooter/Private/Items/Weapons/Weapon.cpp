// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (BoneToHide != FName(""))
	{
		GetItemMesh()->HideBoneByName(BoneToHide, EPhysBodyOp::PBO_None);
	}
	
}


void AWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	const FString WeaponTablePath(TEXT("/Script/Engine.DataTable'/Game/_Game/DataTable/DT_Weapons.DT_Weapons'"));
	UDataTable* WeaponTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *WeaponTablePath));

	if (WeaponTableObject)
	{
		FWeaponDataTable* WeaponDataRow = nullptr;

		switch (WeaponType)
		{
		case EWeaponType::EWT_SubmachineGun:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("SubmachineGun"), TEXT(""));
			break;

		case EWeaponType::EWT_AssaultRifle:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("AssaultRifle"), TEXT(""));
			break;

		case EWeaponType::EWT_Pistol:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("Pistol"), TEXT(""));
			break;
		}

		if (WeaponDataRow)
		{
			AmmoType = WeaponDataRow->AmmoType;
			Ammo = WeaponDataRow->WeaponAmmo;
			MagazineCapacity = WeaponDataRow->MagazineCapacity;
			PickupSound = WeaponDataRow->PickupSound;
			EquipSound = WeaponDataRow->EquipSound;
			GetItemMesh()->SetSkeletalMesh(WeaponDataRow->ItemMesh);
			ItemName = WeaponDataRow->ItemName;
			IconItem = WeaponDataRow->InventoryIcon;
			IconAmmo = WeaponDataRow->AmmoIcon;
			MaterialInstance = WeaponDataRow->MaterialInstance;
			PreviousMaterialIndex = MaterialIndex;
			GetItemMesh()->SetMaterial(PreviousMaterialIndex, nullptr);
			MaterialIndex = WeaponDataRow->MaterialIndex;
			ClipBoneName = WeaponDataRow->ClipBoneName;
			ReloadMontageSection = WeaponDataRow->ReloadMontageSection;
			GetItemMesh()->SetAnimInstanceClass(WeaponDataRow->AnimBP);
			CrosshairMiddle = WeaponDataRow->CrosshairMiddle;
			CrosshairRight = WeaponDataRow->CrosshairRight;
			CrosshairLeft = WeaponDataRow->CrosshairLeft;
			CrosshairTop = WeaponDataRow->CrosshairTop;
			CrosshairBottom = WeaponDataRow->CrosshairBottom;
			AutoFireRate = WeaponDataRow->AutoFireRate;
			MuzzleFlash = WeaponDataRow->MuzzleFlash;
			FireSound = WeaponDataRow->FireSound;
			BoneToHide = WeaponDataRow->BoneToHide;
			bAutomatic = WeaponDataRow->bAutomatic;
			Damage = WeaponDataRow->Damage;
			HeadShotDamage = WeaponDataRow->HeadShotDamage;
			

		}
	}
	if (MaterialInstance)
	{
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MaterialInstance, this);
		DynamicMaterialInstance->SetVectorParameterValue(TEXT("FresnelColor"), GlowColor);
		ItemMesh->SetMaterial(MaterialIndex, DynamicMaterialInstance);
		EnableGlowMaterial();
	}
	
}

void AWeapon::FinishMovingSlide()
{
	bMovingSlide = false;
}

void AWeapon::UpdateSlideDisplacement()
{
	if (SlideDisplacementCurve && bMovingSlide)
	{
		const float ElapstedTime = GetWorldTimerManager().GetTimerElapsed(SlideTimer);
		const float CurveValue = SlideDisplacementCurve->GetFloatValue(ElapstedTime);
		SlideDisplacement = CurveValue * MaxSlideDisplacement;
		RecoilRotation = CurveValue * MaxRecoilRotation;
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Keep Weapon Upright
	if (GetItemState() == EItemState::EIS_Falling && bFalling)
	{
		const FRotator MeshRotation = FRotator(0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f);
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}

	UpdateSlideDisplacement();
}

AWeapon::AWeapon()
{
	ThrowWeaponTime = 0.7f;
	bFalling = false;
}

void AWeapon::ThrowWeapon()
{
	FRotator MeshRotation = FRotator(0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f);
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);

	const FVector MeshForward = FVector(GetItemMesh()->GetForwardVector());
	const FVector MeshRight = FVector(GetItemMesh()->GetRightVector());

	FVector ImpulseDirection = MeshRight.RotateAngleAxis(-20.f, MeshForward);

	double RandomRotation = FMath::FRandRange(20.f, 30.f);
	ImpulseDirection = MeshRight.RotateAngleAxis(RandomRotation, FVector(0.f, 0.f, 1.f));
	ImpulseDirection *= 2000.f;
	GetItemMesh()->AddImpulse(ImpulseDirection);

	bFalling = true;

	GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StopFalling, ThrowWeaponTime);

	EnableGlowMaterial();
}

void AWeapon::DecrementAmmo()
{
	if (Ammo - 1 <= 0)
	{
		Ammo = 0;
	}
	else
	{
		--Ammo;
	}
}

void AWeapon::ReloadAmmo(int32 Amount)
{
	checkf(Ammo + Amount <= MagazineCapacity, TEXT("ATTEMPTED TO RELOAD WITH MORE THAN CAPACITY"));
	Ammo += Amount;
}

bool AWeapon::ClipIsFull()
{
	return Ammo >= MagazineCapacity;
}

void AWeapon::StartSlideTimer()
{
	bMovingSlide = true;
	GetWorldTimerManager().SetTimer(SlideTimer, this, &AWeapon::FinishMovingSlide, SlideDisplacementTime);
}

void AWeapon::StopFalling()
{
	bFalling = false;
	SetITemState(EItemState::EIS_Pickup);
	StartPulseTimer();
}





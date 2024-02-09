// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ShooterCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystemComponent.h"
#include "Items/Item.h"
#include "Components/WidgetComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Sound/SoundBase.h"
#include "Items/Ammo.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Shooter/Shooter.h"
#include "Interfaces/BulletHitinterface.h"
#include "Enemy/Enemy.h"
#include "Particles/ParticleSystem.h"
#include "AI/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Shooter/DebugMacros.h"


AShooterCharacter::AShooterCharacter()
{
	CameraDefaultFOV = 0.f;
	CameraCurrentFOV = 0.f;
	CameraZoomedFOV = 25.f;
	ZoomInterpSpeed = 20.f;

	BaseLookRate = 0.9f;
	HipLookRate = 0.9f;
	AimingLookRate = 0.15f;

	CrosshairSpreadMultiplier = 0.f;
	CrosshairVelocityFactor = 0.f;
	CrosshairInAirFactor = 0.f;
	CrosshairAimFactor = 0.f;
	CrosshairShootingFactor = 0.f;

	ShootTimeDuration = 0.05f;

	bShouldTraceForItems = false;

	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 180.f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SocketOffset = FVector(0.f, 35.f, 80.f);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;

	HandSceneComponent = CreateDefaultSubobject<USceneComponent>((TEXT("HandMoveComponent")));

	WeaponInterpComp = CreateDefaultSubobject<USceneComponent>((TEXT("WeaponInterpolationComponent")));
	WeaponInterpComp->SetupAttachment(Camera);

	ItemInterpComp1 = CreateDefaultSubobject<USceneComponent>((TEXT("ItemInterpolationComponent1")));
	ItemInterpComp1->SetupAttachment(Camera);

	ItemInterpComp2 = CreateDefaultSubobject<USceneComponent>((TEXT("ItemInterpolationComponent2")));
	ItemInterpComp2->SetupAttachment(Camera);

	ItemInterpComp3 = CreateDefaultSubobject<USceneComponent>((TEXT("ItemInterpolationComponent3")));
	ItemInterpComp3->SetupAttachment(Camera);

	ItemInterpComp4 = CreateDefaultSubobject<USceneComponent>((TEXT("ItemInterpolationComponent4")));
	ItemInterpComp4->SetupAttachment(Camera);

	ItemInterpComp5 = CreateDefaultSubobject<USceneComponent>((TEXT("ItemInterpolationComponent5")));
	ItemInterpComp5->SetupAttachment(Camera);

	ItemInterpComp6 = CreateDefaultSubobject<USceneComponent>((TEXT("ItemInterpolationComponent6")));
	ItemInterpComp6->SetupAttachment(Camera);

}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Add Input
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(CharacterMappingContext, 0);
		}
	}

	if (Camera)
	{
		CameraDefaultFOV = Camera->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}

	EquipWeapon(SpawnDefaultWeapon());
	Inventory.Add(EquippedWeapon);
	EquippedWeapon->SetSlotIndex(0);

	EquippedWeapon->DisableCustomDepth();
	EquippedWeapon->DisableGlowMaterial();
	EquippedWeapon->SetCharacter(this);
	InitializeAmmoMap();
	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	InitializeInterpLocations();
	
}

void AShooterCharacter::Jump()
{
	if (bCrouching)
	{
		bCrouching = false;
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
	else
	{
		Super::Jump();
	}
}

void AShooterCharacter::Move(const FInputActionValue& Value)
{
	FVector2D DirectionVector = Value.Get<FVector2D>();

	if (Controller)
	{
		//Finds out which direction is forward using controller as base.
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		//get forward vector of the controller
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		//adds the movement input forward vector * the direction
		AddMovementInput(ForwardDirection, DirectionVector.Y);

		//get Right Vector of the controller
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		//adds the movement input right vector * the direction
		AddMovementInput(RightDirection, DirectionVector.X);
	}
}

void AShooterCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();

	SetLookRate();

	if (Controller)
	{

		AddControllerYawInput(LookVector.X * BaseLookRate);
		AddControllerPitchInput(LookVector.Y * BaseLookRate);
	}

}

void AShooterCharacter::SetLookRate()
{
	if (isZooming())
	{
		BaseLookRate = AimingLookRate;
	}
	else
	{
		BaseLookRate = HipLookRate;
	}

	BaseLookRate = FMath::Clamp(BaseLookRate, 0.f, 1.f);
}

void AShooterCharacter::FireWeapon()
{
	if (!EquippedWeapon) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (WeaponHasAmmo())
	{
		
		SendBullet();
		PlayMontage(HipFireMontage, FName("Fire"));
		PlayGunshotSound(); 
		StartCrosshairBulletFire();
		EquippedWeapon->DecrementAmmo();
		StartFireTimer();

		if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol)
		{
			EquippedWeapon->StartSlideTimer();
		}
	}

}

void AShooterCharacter::SendBullet()
{
	const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName("BarrelSocket");
	if (!BarrelSocket) return;

	const FTransform SocketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());
	FHitResult BeamHitResult;

	SpawnMuzzleParticles(BarrelSocket);

	bool bBeamEnd = GetBeamEndLocation(BeamHitResult, SocketTransform.GetLocation());
	SpawnBeamParticles(SocketTransform, BeamHitResult);

	if (!bBeamEnd) return;
	if (!BeamHitResult.GetActor()) return;

	IBulletHitinterface* BulletHitInterface = Cast<IBulletHitinterface>(BeamHitResult.GetActor());

	if (BulletHitInterface)
	{
		BulletHitInterface->BulletHit_Implementation(BeamHitResult, this, GetController());
	}

	AEnemy* HitEnemy = Cast<AEnemy>(BeamHitResult.GetActor());

	if (HitEnemy)
	{
		int32 Damage = 0;
		if (BeamHitResult.BoneName.ToString() == HitEnemy->GetHeadBone())
		{
			Damage = EquippedWeapon->GetHeadShotDamage();
			UGameplayStatics::ApplyDamage(BeamHitResult.GetActor(), EquippedWeapon->GetHeadShotDamage(), GetController(), this, UDamageType::StaticClass());
			HitEnemy->ShowHitNumber(Damage, BeamHitResult.Location, true);
		}
		else
		{
			Damage = EquippedWeapon->GetDamage();
			UGameplayStatics::ApplyDamage(BeamHitResult.GetActor(), EquippedWeapon->GetDamage(), GetController(), this, UDamageType::StaticClass());
			HitEnemy->ShowHitNumber(Damage, BeamHitResult.Location, false);
		}

		
		
	}

	else
	{
		SpawnImpactParticles(BeamHitResult);
	}
	
}

bool AShooterCharacter::GetBeamEndLocation(FHitResult& BeamEndResult, const FVector& SocketTransform)
{
	FVector OutBeamLocation;
	FHitResult CrosshairHitResult;
	TraceUnderCrosshairs(CrosshairHitResult, OutBeamLocation);

	//if (bCrosshairHit)
	//{
	//	OutBeamLocation = CrosshairHitResult.Location;
	//}

	return WeaponLineTrace(SocketTransform, BeamEndResult, OutBeamLocation);
	
}

void AShooterCharacter::SpawnImpactParticles(const FHitResult& BeamEndPoint)
{
	if (WeaponImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
			WeaponImpactParticles,
			BeamEndPoint.Location);
	}
}

void AShooterCharacter::SpawnBeamParticles(const FTransform& SocketTransform, const FHitResult& BeamEndPoint)
{
	if (BeamParticles)
	{
		UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, SocketTransform);
		if (!Beam) return;

		Beam->SetVectorParameter(FName("Target"), BeamEndPoint.Location);

	}

}

bool AShooterCharacter::WeaponLineTrace(const FVector& SocketTransform, FHitResult& BeamEndResult, FVector& BeamEndPoint)
{
	//FHitResult WeaponTraceHit;
	const FVector WeaponTraceStart = SocketTransform;
	const FVector StartToEnd = BeamEndPoint - SocketTransform;
	const FVector WeaponTraceEnd = BeamEndPoint + StartToEnd * 1.25f;

	GetWorld()->LineTraceSingleByChannel(
		BeamEndResult,
		WeaponTraceStart,
		WeaponTraceEnd,
		ECollisionChannel::ECC_Visibility);

	if (!BeamEndResult.bBlockingHit)
	{
		BeamEndResult.Location = BeamEndPoint;
		return false;
	}
	return true;
}

void AShooterCharacter::GetViewPortSize(FVector2D& ViewportSize)
{
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
}

bool AShooterCharacter::isScreenToWorld(FVector& CrosshairWorldPosition, FVector& CrosshairWorldDirection)
{
	FVector2D ViewportSize;
	GetViewPortSize(ViewportSize);

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	CrosshairLocation.Y -= 50.f;


	return UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);
}

void AShooterCharacter::PlayMontage(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AShooterCharacter::PlayGunshotSound()
{
	if (EquippedWeapon->GetFireSound())
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquippedWeapon->GetFireSound(),
			GetActorLocation()
		);
	}
}

void AShooterCharacter::SpawnMuzzleParticles(const USkeletalMeshSocket* Socket)
{

	if (Socket && EquippedWeapon->GetMuzzleFlash())
	{
		//const FTransform SocketTransform = Socket->GetSocketTransform(GetMesh());
		UGameplayStatics::SpawnEmitterAttached(
			EquippedWeapon->GetMuzzleFlash(),
			EquippedWeapon->GetItemMesh(),
			Socket->SocketName
		);

	}
}

bool AShooterCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation)
{
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	if (isScreenToWorld(CrosshairWorldPosition, CrosshairWorldDirection))
	{

		FVector Start = CrosshairWorldPosition;
		FVector End = Start + CrosshairWorldDirection * GunLineTraceLength;
		OutHitLocation = End;

		GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECollisionChannel::ECC_Visibility);

		if (OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
		}
		return true;
	}
	return false;
}

AWeapon* AShooterCharacter::SpawnDefaultWeapon()
{
	if (DefaultWeaponClass)
	{
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}
	return nullptr;
}

void AShooterCharacter::EquipWeapon(AWeapon* WeaponToEquip, bool bSwapping)
{
	if (WeaponToEquip)
	{

		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}

		if (EquippedWeapon == nullptr)
		{
			EquipItemDelegate.Broadcast(-1, WeaponToEquip->GetSlotIndex());
		}
		else if (!bSwapping)
		{
			EquipItemDelegate.Broadcast(EquippedWeapon->GetSlotIndex(), WeaponToEquip->GetSlotIndex());
		}

		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetITemState(EItemState::EIS_Equipped);
	}
}

void AShooterCharacter::DropWeapon()
{
	if (EquippedWeapon)
	{
		FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);
		EquippedWeapon->SetITemState(EItemState::EIS_Falling);
		EquippedWeapon->ThrowWeapon();
	}
}

void AShooterCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	if (Inventory.Num() - 1 >= EquippedWeapon->GetSlotIndex())
	{
		Inventory[EquippedWeapon->GetSlotIndex()] = WeaponToSwap;
		WeaponToSwap->SetSlotIndex(EquippedWeapon->GetSlotIndex());
	}
	DropWeapon();
	EquipWeapon(WeaponToSwap, true);
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}

void AShooterCharacter::PickupAmmo(AAmmo* Ammo)
{
	if (AmmoMap.Find(Ammo->GetAmmoType()))
	{
		//Get amount of ammo in our AMmoMap for AMmo's type
		int32 AmmoCount = AmmoMap[Ammo->GetAmmoType()];
		AmmoCount += Ammo->GetItemCount();
		//Set the amount of ammo in the Map for this type
		AmmoMap[Ammo->GetAmmoType()] = AmmoCount;
	}

	if (EquippedWeapon->GetAmmoType() == Ammo->GetAmmoType())
	{
		//Check to see if gun is empty
		if (EquippedWeapon->GetAmmo() == 0)
		{
			ReloadWeapon();
		}
	}

	Ammo->Destroy();
}

bool AShooterCharacter::WeaponHasAmmo()
{
	if (EquippedWeapon == nullptr) return false;

	return EquippedWeapon->GetAmmo() > 0;
}

void AShooterCharacter::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_9mm, 120);
	AmmoMap.Add(EAmmoType::EAT_AR, 90);
}

bool AShooterCharacter::CarryingAmmo()
{
	if (!EquippedWeapon) return false;

	EAmmoType AmmoType = EquippedWeapon->GetAmmoType();

	if (AmmoMap.Contains(AmmoType))
	{
		return AmmoMap[AmmoType] > 0;
	}

	return false;
}

void AShooterCharacter::InterpCapsuleHalfHeight(float DeltaTime)
{
	float TargetCapsuleHalfHeight;
	CurrentCapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	if (bCrouching)
	{
		TargetCapsuleHalfHeight = CrouchingCapsuleHalfHeight;
	}

	else
	{
		TargetCapsuleHalfHeight = StandingCapsuleHalfHeight;
	}

	
	const float InterpHalfHeight = FMath::FInterpTo(CurrentCapsuleHalfHeight, TargetCapsuleHalfHeight, DeltaTime, 20.f);
	//Negative value = crouching, positive value = standing
	const float DeltaCapsuleHalfHeight = InterpHalfHeight - CurrentCapsuleHalfHeight;
	const FVector MeshOffset = FVector(0.f, 0.f, -DeltaCapsuleHalfHeight);
	GetMesh()->AddLocalOffset(MeshOffset);

	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHalfHeight);
}

void AShooterCharacter::AimingButtonPressed()
{
	bAimingButtonPressed = true;
	if (CombatState != ECombatState::ECS_Reloading && CombatState != ECombatState::ECS_Equipping && CombatState != ECombatState::ECS_Stunned)
	{
		Aim();
	}
	
}

void AShooterCharacter::AimingButtonReleased()
{
	bAimingButtonPressed = false;
	StopAiming();
}

void AShooterCharacter::SelectButtonPressed()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (TraceHitItem)
	{
		TraceHitItem->StartItemCurve(this, true);
		TraceHitItem = nullptr;
	}
	
}

void AShooterCharacter::SelectButtonReleased()
{
}

void AShooterCharacter::FireButtonPressed()
{
	BulletState = EBulletState::EBS_Fire;
	FireWeapon();
	
}

void AShooterCharacter::Aim()
{
	ZoomState = EZoomState::EZS_ZoomAim;
	GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
}

void AShooterCharacter::FireButtonReleased()
{
	BulletState = EBulletState::EBS_NoFire;
}

void AShooterCharacter::StopAiming()
{
	ZoomState = EZoomState::EZS_DefaultAim;

	if (!bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
}

void AShooterCharacter::ReloadButtonPressed()
{
	ReloadWeapon();
}

void AShooterCharacter::InitializeInterpLocations()
{
	FInterpLocation WeaponLocation{ WeaponInterpComp, 0 };
	InterpLocations.Add(WeaponLocation);

	FInterpLocation InterpLoc1{ ItemInterpComp1, 0 };
	InterpLocations.Add(InterpLoc1);

	FInterpLocation InterpLoc2{ ItemInterpComp2, 0 };
	InterpLocations.Add(InterpLoc2);

	FInterpLocation InterpLoc3{ ItemInterpComp3, 0 };
	InterpLocations.Add(InterpLoc3);

	FInterpLocation InterpLoc4{ ItemInterpComp4, 0 };
	InterpLocations.Add(InterpLoc4);

	FInterpLocation InterpLoc5{ ItemInterpComp5, 0 };
	InterpLocations.Add(InterpLoc5);

	FInterpLocation InterpLoc6{ ItemInterpComp6, 0 };
	InterpLocations.Add(InterpLoc6);
}

void AShooterCharacter::ReloadWeapon()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (!EquippedWeapon) return;

	if (CarryingAmmo() && !EquippedWeapon->ClipIsFull()) 
	{
		if (ZoomState == EZoomState::EZS_ZoomAim)
		{
			StopAiming();
		}

		CombatState = ECombatState::ECS_Reloading;
		PlayMontage(ReloadMontage, EquippedWeapon->GetReloadMontageSection());
	}
}

void AShooterCharacter::CrouchButtonPressed()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		bCrouching = !bCrouching;
	}

	if (bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
		GetCharacterMovement()->GroundFriction = CrouchingGroundFriction;
		
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
		GetCharacterMovement()->GroundFriction = BaseGroundFriction;
	}
}

void AShooterCharacter::FinishEquipping()
{
	if (CombatState == ECombatState::ECS_Stunned) return;

	CombatState = ECombatState::ECS_Unoccupied;

	if (bAimingButtonPressed)
	{
		Aim();
	}
}

void AShooterCharacter::FinishReloading()
{
	if (CombatState == ECombatState::ECS_Stunned) return;

	CombatState = ECombatState::ECS_Unoccupied;

	if (ZoomState == EZoomState::EZS_ZoomAim)
	{
		Aim();
	}

	if (!EquippedWeapon) return;

	const EAmmoType AmmoType = EquippedWeapon->GetAmmoType();

	if (AmmoMap.Contains(AmmoType))
	{
		//Amount of ammo the Character is carrying for the curretn EquippedWeapon
		int32 CarriedAmmo = AmmoMap[AmmoType];
		//Space left in the magazine of equipped weapon
		const int32 MagEmptySpace = EquippedWeapon->GetMagazineCapacity() - EquippedWeapon->GetAmmo();

		if (MagEmptySpace > CarriedAmmo)
		{
			//Reload the magazine with all the ammo we are carrying
			EquippedWeapon->ReloadAmmo(CarriedAmmo);
			CarriedAmmo = 0;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
		else
		{
			//fill the magazine
			EquippedWeapon->ReloadAmmo(MagEmptySpace);
			CarriedAmmo -= MagEmptySpace;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
	}
}

void AShooterCharacter::GrabClip()
{
	if (!EquippedWeapon) return;
	if (!HandSceneComponent) return;
	
	int32 ClipBoneIndex = EquippedWeapon->GetItemMesh()->GetBoneIndex(EquippedWeapon->GetClipBoneName());
	ClipTransform = EquippedWeapon->GetItemMesh()->GetBoneTransform(ClipBoneIndex);

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);

	HandSceneComponent->AttachToComponent(GetMesh(), AttachmentRules, FName(TEXT("hand_l")));
	HandSceneComponent->SetWorldTransform(ClipTransform);
	EquippedWeapon->SetMovingClip(true);
	
}

void AShooterCharacter::ReleaseClip()
{
	EquippedWeapon->SetMovingClip(false);
}

void AShooterCharacter::SwitchButtonPressed(const FInputActionValue& Value)
{
	float Index = Value.Get<float>() - 1;

	if (EquippedWeapon->GetSlotIndex() == Index) return;

	ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), Index);
}


void AShooterCharacter::ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex)
{
	
	const bool bCanExchangeItems = (CurrentItemIndex != NewItemIndex) && 
		(NewItemIndex < Inventory.Num()) && 
		(CombatState == ECombatState::ECS_Unoccupied || CombatState == ECombatState::ECS_Equipping);

	if(bCanExchangeItems)
	{
		if (ZoomState == EZoomState::EZS_ZoomAim)
		{
			StopAiming();
		}

		AWeapon* OldEquippedWeapon = EquippedWeapon;
		AWeapon* NewWeapon = Cast<AWeapon>(Inventory[NewItemIndex]);
		EquipWeapon(NewWeapon);
		OldEquippedWeapon->SetITemState(EItemState::EIS_Pickedup);
		NewWeapon->SetITemState(EItemState::EIS_Equipped);

		CombatState = ECombatState::ECS_Equipping;

		PlayMontage(EquippingMontage, FName("Equip"));

		NewWeapon->PlayEquipSound(true);
	}
}

void AShooterCharacter::ResetPickupSoundTimer()
{
	bShouldPlayPickupSound = true;
}

void AShooterCharacter::ResetEquipSoundTimer()
{
	bShouldPlayEquipSound = true;
}

void AShooterCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	FVector2D WalkSpeedRange = FVector2D(0.f, 600.f);
	FVector2D VelocityMultiplierRange = FVector2D(0.f, 1.f);
	FVector Velocity = FVector(GetVelocity().X,GetVelocity().Y, 0.f);

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

	if (GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	else
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
	}

	if (isZooming())
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.5f, DeltaTime, 15.f);
	}
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
	}

	if (!bShouldFire)
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.3f, DeltaTime, 60.f);
	}
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 60.f);
	}

	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor + CrosshairInAirFactor - CrosshairAimFactor + CrosshairShootingFactor;
}

void AShooterCharacter::HighlightInventorySlot()
{
	const int32 EmptySlot = GetEmptyInventorySlot();
	HighlightIconDelegate.Broadcast(EmptySlot, true);
	HighlightedSlot = EmptySlot;
}

int32 AShooterCharacter::GetEmptyInventorySlot()
{
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i] == nullptr)
		{
			return i;
		}
	}

	if (Inventory.Num() < INVENTORY_CAPACITY)
	{
		return Inventory.Num();
	}
	return -1; //inventory is full
}



void AShooterCharacter::EndStun()
{
	CombatState = ECombatState::ECS_Unoccupied;

	if (bAimingButtonPressed)
	{
		Aim();
	}
}

EPhysicalSurface AShooterCharacter::GetSurfaceType()
{
	FHitResult HitResult;
	const FVector Start = GetActorLocation();
	const FVector End = Start + FVector(0.f, 0.f, -400.f);
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, QueryParams);

	return UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
	
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Zoom when Aiming
	SetCameraFieldOfView(DeltaTime);
	CalculateCrosshairSpread(DeltaTime);
	TraceForItems();
	InterpCapsuleHalfHeight(DeltaTime);
}

void AShooterCharacter::TraceForItems()
{
	if (bShouldTraceForItems)
	{
		FHitResult ItemTraceResult;
		FVector OutHit;
		TraceUnderCrosshairs(ItemTraceResult, OutHit);

		if (ItemTraceResult.bBlockingHit)
		{
			TraceHitItem = Cast<AItem>(ItemTraceResult.GetActor());
			const AWeapon* TraceHitWeapon = Cast<AWeapon>(TraceHitItem);

			if (TraceHitWeapon)
			{
				if (HighlightedSlot == -1)
				{
					//Highlight a inventory slot
					HighlightInventorySlot();
				}
			}
			else
			{
				//is a slot being highlighted
				if (HighlightedSlot != -1)
				{
					//unhighlight inventory slot
					UnHighlightInventorySlot();
				}
			}

			if (TraceHitItem && TraceHitItem->GetItemState() == EItemState::EIS_EquippedInterping)
			{
				TraceHitItem = nullptr;
			}

			if (TraceHitItem && TraceHitItem->GetPickupWidget())
			{
				TraceHitItem->GetPickupWidget()->SetVisibility(true);
				TraceHitItem->EnableCustomDepth();

				if (Inventory.Num() >= INVENTORY_CAPACITY)
				{
					TraceHitItem->SetCharacterInventoryFull(true);
				}
				else
				{
					TraceHitItem->SetCharacterInventoryFull(false);
				}
			}

			if (TraceHitItemLastFrame)
			{
				if (TraceHitItem != TraceHitItemLastFrame)
				{
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
					TraceHitItemLastFrame->DisableCustomDepth();
				}
			}

			TraceHitItemLastFrame = TraceHitItem;
		}
	}

	else if (TraceHitItemLastFrame)
	{
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
		TraceHitItemLastFrame->DisableCustomDepth();
	}
}

void AShooterCharacter::SetCameraFieldOfView(float DeltaTime)
{
	if (isZooming())
	{
		//Interpolates to Zoomed Camera
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);
	}
	else
	{
		//Interpolates to Default Camera
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
	}

	Camera->SetFieldOfView(CameraCurrentFOV);
}

/*FVector AShooterCharacter::GetCameraInterpLocation()
{
	const FVector CameraWorldLocation = Camera->GetComponentLocation();
	const FVector CameraForwardVector = Camera->GetForwardVector();
	const FVector CameraUpVector = Camera->GetUpVector();
	
	FVector WeaponToCameraInterpLocation = CameraWorldLocation + (CameraForwardVector * CameraInterpDistance) + (CameraUpVector * CameraInterpElevation);

	return WeaponToCameraInterpLocation;
}
*/
bool AShooterCharacter::isZooming()
{
	return ZoomState == EZoomState::EZS_ZoomAim;
}

void AShooterCharacter::UnHighlightInventorySlot()
{
	HighlightIconDelegate.Broadcast(HighlightedSlot, false);
	HighlightedSlot = -1;
}

void AShooterCharacter::Stun()
{
	if (Health <= 0) return;

	CombatState = ECombatState::ECS_Stunned;
	PlayMontage(HitReactMontage, FName(TEXT("HitFront")));
}

bool AShooterCharacter::isBulletFiring()
{
	return BulletState == EBulletState::EBS_Fire;
}

bool AShooterCharacter::isNotBulletFiring()
{
	return BulletState == EBulletState::EBS_NoFire;
}

void AShooterCharacter::StartCrosshairBulletFire()
{
	if (bShouldFire)
	{
		GetWorldTimerManager().SetTimer(
		CrosshairShootTimer,
		this,
		&AShooterCharacter::FinishCrosshairBulletFire,
		ShootTimeDuration);
	}
	
}

void AShooterCharacter::StartFireTimer()
{
	if (!EquippedWeapon) return;

	CombatState = ECombatState::ECS_FireTimerInProgress;
	bShouldFire = false;
	GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AShooterCharacter::AutoFireReset, EquippedWeapon->GetAutoFireRate());
}

void AShooterCharacter::FinishCrosshairBulletFire()
{
	bShouldFire = true;
}

void AShooterCharacter::AutoFireReset()
{
	if (CombatState == ECombatState::ECS_Stunned) return;

	CombatState = ECombatState::ECS_Unoccupied;
	if (EquippedWeapon == nullptr) return;

	if (WeaponHasAmmo())
	{
		if (BulletState == EBulletState::EBS_Fire && EquippedWeapon->GetAutomatic())
		{
			FireWeapon();
		}
	}

	else
	{
		ReloadWeapon();
	}
	
}

FInterpLocation AShooterCharacter::GetInterpLocation(int32 Index)
{
	if (Index <= InterpLocations.Num())
	{
		return InterpLocations[Index];
	}
	return FInterpLocation();
}

void AShooterCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}

	else
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
}

float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

void AShooterCharacter::IncrementInterpLocItemCount(int32 Index, int32 Amount)
{
	if (Amount < -1 || Amount > 1) return;

	if (InterpLocations.Num() >= Index)
	{
		InterpLocations[Index].ItemCount += Amount;
	}
}

void AShooterCharacter::GetPickupItem(AItem* Item)
{
	Item->PlayEquipSound();

	AWeapon* Weapon = Cast<AWeapon>(Item);

	if (Weapon)
	{
		if (Inventory.Num() < INVENTORY_CAPACITY)
		{
			Weapon->SetSlotIndex(Inventory.Num());
			Inventory.Add(Weapon);
			Weapon->SetITemState(EItemState::EIS_Pickedup);
		}
		else
		{
			SwapWeapon(Weapon);
		}
		
	}

	AAmmo* Ammo = Cast<AAmmo>(Item);

	if (Ammo)
	{
		PickupAmmo(Ammo);
	}
}

void AShooterCharacter::StartPickupSoundTimer()
{
	bShouldPlayPickupSound = false;

	GetWorldTimerManager().SetTimer(
		PickupSoundTimer,
		this,
		&AShooterCharacter::ResetPickupSoundTimer,
		PickupSoundResetTime);
}

void AShooterCharacter::StartEquipSoundTimer()
{
	bShouldPlayEquipSound = false;

	GetWorldTimerManager().SetTimer(
		EquipSoundTimer,
		this,
		&AShooterCharacter::ResetEquipSoundTimer,
		EquipSoundResetTime);
}

int32 AShooterCharacter::GetInterpLocationIndex()
{
	//returns the index in the InterpLocations array with the lowest ItemCount
	int32 LowestIndex = 1;
	int32 LowestCount = INT_MAX;

	for (int32 i = 1; i < InterpLocations.Num(); i++)
	{
		if (InterpLocations[i].ItemCount < LowestCount)
		{
			LowestIndex = i;
			LowestCount = InterpLocations[i].ItemCount;
		}
	}

	return LowestIndex;
}

void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &AShooterCharacter::FireButtonPressed);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Completed, this, &AShooterCharacter::FireButtonReleased);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &AShooterCharacter::AimingButtonPressed);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AShooterCharacter::AimingButtonReleased);
		EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Triggered, this, &AShooterCharacter::SelectButtonPressed);
		EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Completed, this, &AShooterCharacter::SelectButtonReleased);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AShooterCharacter::ReloadButtonPressed);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AShooterCharacter::CrouchButtonPressed);
		EnhancedInputComponent->BindAction(SwitchAction, ETriggerEvent::Triggered, this, &AShooterCharacter::SwitchButtonPressed);
	}

}

float AShooterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		Die();

		AEnemyAIController* EnemyController = Cast<AEnemyAIController>(EventInstigator);

		if (EnemyController)
		{
			EnemyController->GetBlackBoardComponent()->SetValueAsBool(FName("CharacterDead"), true);
		}
	}

	else
	{
		Health -= DamageAmount;
	}
	return DamageAmount;
}

void AShooterCharacter::Die()
{	
	if (bDying == true) return;
	bDying = true;
	const int DeathRandom = FMath::RandRange(0, MontageDeaths.Num() - 1);
	PlayMontage(DeathMontage, MontageDeaths[DeathRandom]);
}

void AShooterCharacter::FinishDeath()
{
	GetMesh()->bPauseAnims = true;
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

	if (PC)
	{
		DisableInput(PC);
	}
}

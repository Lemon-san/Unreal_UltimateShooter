// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Character/ShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Sound/SoundBase.h"
#include <Kismet/GameplayStatics.h>
#include "Curves/CurveVector.h"

// Sets default values
AItem::AItem()
{
	ItemName = FString("Default");
	ItemCount = 0;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	RootComponent = ItemMesh;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(GetRootComponent());
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("WeaponPickup"));
	PickupWidget->SetupAttachment(GetRootComponent());

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
	SetActiveStars();
	SetItemProperties(ItemState);
	InitializeCustomDepth();
	StartPulseTimer();

	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnEndOverlap);

}

void AItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	//Load Data in the ItemRariry Data Table

	//Path to the item rarity data table
	FString RariryTablePath(TEXT("/Script/Engine.DataTable'/Game/_Game/DataTable/DT_ItemRarirty.DT_ItemRarirty'"));
	UDataTable* RarityTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *RariryTablePath));

	if (RarityTableObject)
	{
		FItemRarityTable* RarityRow = nullptr;
		switch (WeaponRarity)
		{
		case EBulletRarity::EBR_Damaged:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Damaged"), TEXT(""));
			break;
		case EBulletRarity::EBR_Common:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Common"), TEXT(""));
			break;
		case EBulletRarity::EBR_Uncommon:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Uncommon"), TEXT(""));
			break;
		case EBulletRarity::EBR_Rare:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Rare"), TEXT(""));
			break;
		case EBulletRarity::EBR_Legendary:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Legendary"), TEXT(""));
			break;
		}

		if (RarityRow)
		{
			GlowColor = RarityRow->GlowColor;
			LightColor = RarityRow->LightColor;
			DarkColor = RarityRow->DarkColor;
			NumberOfStars = RarityRow->NumberOfStars;
			IconBackground = RarityRow->IconBackground;

			if (GetItemMesh())
			{
				GetItemMesh()->SetCustomDepthStencilValue(RarityRow->CustomDepthStencil);
			}
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

void AItem::CurrentActiveStars(int32 TotalStars)
{
	for (int i = 0; i <= TotalStars; i++)
	{
		ActiveStars[i] = true;
	}
}

void AItem::SetActiveStars()
{
	for (int i = 0; i <= 4; i++)
	{
		ActiveStars.Add(false);
	}

	switch (WeaponRarity)
	{
	case EBulletRarity::EBR_Damaged:
		CurrentActiveStars(0);
		break;
	case EBulletRarity::EBR_Common:
		CurrentActiveStars(0);
		CurrentActiveStars(1);
		break;
	case EBulletRarity::EBR_Uncommon:
		CurrentActiveStars(0);
		CurrentActiveStars(1);
		CurrentActiveStars(2);
		break;
	case EBulletRarity::EBR_Rare:
		CurrentActiveStars(0);
		CurrentActiveStars(1);
		CurrentActiveStars(2);
		CurrentActiveStars(3);
		break;
	case EBulletRarity::EBR_Legendary:
		CurrentActiveStars(0);
		CurrentActiveStars(1);
		CurrentActiveStars(2);
		CurrentActiveStars(3);
		CurrentActiveStars(4);
		break;
	}
}

void AItem::SetItemProperties(EItemState State)
{
	switch (State)
	{
	case EItemState::EIS_Pickup:
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		break;
	case EItemState::EIS_Equipped:
		PickupWidget->SetVisibility(false);
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;
	case EItemState::EIS_Falling:
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		

		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;
	case EItemState:: EIS_EquippedInterping:
		PickupWidget->SetVisibility(false);

		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::EIS_Pickedup:
		PickupWidget->SetVisibility(false);

		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(false);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

void AItem::ItemInterp(float DeltaTime)
{
	if (!bInterping) return;

	if (PlayerCharacter && ItemZCurve)
	{
		//Elapsed time since we started ItemInterpTimer
		const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
		//Get Curve value corresponding to ElapsedTime
		const float CurveValue = ItemZCurve->GetFloatValue(ElapsedTime);
		//get item initial location when the curve starts
		FVector ItemLocation = ItemInterpStartLocation;
		//Get Location in front of camera
		const FVector CameraInterpLocation = GetInterpLocation();

		//Vector from item to camera interp location
		const FVector ItemToCamera = FVector(0.f, 0.f, (CameraInterpLocation - ItemLocation).Z);
		const float DeltaZ = ItemToCamera.Size();

		//Interoplated X and Y values
		const FVector CurrentLocation = GetActorLocation();
		const float InterpXValue = FMath::FInterpTo(CurrentLocation.X, CameraInterpLocation.X, DeltaTime, 30.f);
		const float InterpYValue = FMath::FInterpTo(CurrentLocation.Y, CameraInterpLocation.Y, DeltaTime, 30.f);

		//Set X and Y of itemLocation to interp values
		ItemLocation.X = InterpXValue;
		ItemLocation.Y = InterpYValue;

		//Addinng curve value to the z component of the initial Location scaled by DeltaZ
		ItemLocation.Z += CurveValue * DeltaZ;
		SetActorLocation(ItemLocation, true, nullptr, ETeleportType::TeleportPhysics);

		const FRotator CameraRotation = PlayerCharacter->GetCamera()->GetComponentRotation();
		FRotator ItemRotation = FRotator(0.f, CameraRotation.Yaw + InterpInitialYawOffset, 0.f);
		SetActorRotation(ItemRotation, ETeleportType::TeleportPhysics);

		if (ItemScaleCurve)
		{
			const float ScaleCurveValue = ItemScaleCurve->GetFloatValue(ElapsedTime);
			SetActorScale3D(FVector(ScaleCurveValue, ScaleCurveValue, ScaleCurveValue));
		}
		

	}
}

void AItem::FinishInterping()
{
	bInterping = false;
	if (PlayerCharacter)
	{
		//substract 1 from the item count of the interp location struct
		PlayerCharacter->IncrementInterpLocItemCount(InterpLocIndex, -1);
		PlayerCharacter->GetPickupItem(this);

		PlayerCharacter->UnHighlightInventorySlot();
	}

	//Sets scale back to normal
	SetActorScale3D(FVector(1.f));

	DisableGlowMaterial();
	bCanChangeCustomDepth = true;
	DisableCustomDepth();

}

void AItem::PlayPickupSound(bool bForcePlaySound)
{
	if (PlayerCharacter)
	{
		if (bForcePlaySound)
		{
			if (PickupSound)
			{
				UGameplayStatics::PlaySound2D(this, PickupSound);
			}
		}
		else if (PlayerCharacter->GetShouldPlayPickupSound())
		{
			PlayerCharacter->StartPickupSoundTimer();
			if (PickupSound)
			{
				UGameplayStatics::PlaySound2D(this, PickupSound);
			}
		}
		
	}
}

void AItem::ResetPulseTimer()
{
	StartPulseTimer();
}

void AItem::StartPulseTimer()
{
	if (ItemState == EItemState::EIS_Pickup)
	{
		GetWorldTimerManager().SetTimer(PulseTimer, this, &AItem::ResetPulseTimer, PulseCurveTime);
	}
}

void AItem::UpdatePulse()
{
	float ElapsedTime;
	FVector CurveValue;

	switch (ItemState)
	{
	case EItemState::EIS_Pickup:
		if (PulseCurve)
		{
			ElapsedTime = GetWorldTimerManager().GetTimerElapsed(PulseTimer);
			CurveValue = PulseCurve->GetVectorValue(ElapsedTime);
		}
		break;
	case EItemState::EIS_EquippedInterping:
		if (InterpPulseCurve)
		{
			ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
			CurveValue = InterpPulseCurve->GetVectorValue(ElapsedTime);
		}
		break;
	}

	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowAmount"), CurveValue.X * GlowAmount);
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("FresnelExponent"), CurveValue.Y * FresnelExponent);
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("FresnelReflectFraction"), CurveValue.Z * FresnelReflectFraction);
	}
	
}

void AItem::EnableGlowMaterial()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 0);
	}
}

void AItem::DisableGlowMaterial()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 1);
	}
}

void AItem::PlayEquipSound(bool bForcePlaySound)
{
	if (PlayerCharacter)
	{
		if (bForcePlaySound)
		{
			if (EquipSound)
			{
				UGameplayStatics::PlaySound2D(this, EquipSound);
			}
		}
		
		else if (PlayerCharacter->GetShouldPlayEquipSound())
		{
			PlayerCharacter->StartEquipSoundTimer();
			if (EquipSound)
			{
				UGameplayStatics::PlaySound2D(this, EquipSound);
			}
		}
	}
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);

		if (ShooterCharacter)
		{
			ShooterCharacter->IncrementOverlappedItemCount(1);
			
		}
	}
}

void AItem::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);

		if (ShooterCharacter)
		{
			ShooterCharacter->IncrementOverlappedItemCount(-1);
			ShooterCharacter->UnHighlightInventorySlot();
		}
	}
}

void AItem::EnableCustomDepth()
{
	if (bCanChangeCustomDepth)
	{
		ItemMesh->SetRenderCustomDepth(true);
	}
	
}

void AItem::DisableCustomDepth()
{
	if (bCanChangeCustomDepth)
	{
		ItemMesh->SetRenderCustomDepth(false);
	}
}

void AItem::InitializeCustomDepth()
{
	DisableCustomDepth();
}

FVector AItem::GetInterpLocation()
{
	if (PlayerCharacter == nullptr) return FVector(0.f);

	switch (ItemType)
	{
	case EItemType::EIT_Ammo:
		return PlayerCharacter->GetInterpLocation(InterpLocIndex).SceneComponent->GetComponentLocation();
		break;
	case EItemType::EIT_Weapon:
		return PlayerCharacter->GetInterpLocation(0).SceneComponent->GetComponentLocation();
		break;
	}

	return FVector();
}

void AItem::SetITemState(EItemState State)
{
	ItemState = State;
	SetItemProperties(State);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ItemInterp(DeltaTime);
	UpdatePulse();

}

void AItem::StartItemCurve(AShooterCharacter* Character, bool bForcePlaySound)
{
	//Stores a a handle to the character
	PlayerCharacter = Character;
	
	//Get array index in InterpLocations with the lowest ItemCount
	InterpLocIndex = PlayerCharacter->GetInterpLocationIndex();
	//Add 1 to the item count for this inter location struct
	PlayerCharacter->IncrementInterpLocItemCount(InterpLocIndex, 1);

	ItemInterpStartLocation = GetActorLocation();
	bInterping = true;
	SetITemState(EItemState::EIS_EquippedInterping);
	GetWorldTimerManager().ClearTimer(PulseTimer);
	GetWorldTimerManager().SetTimer(ItemInterpTimer, this, &AItem::FinishInterping, ZCurveTime);

	const float CameraRotationYaw = PlayerCharacter->GetCamera()->GetComponentRotation().Yaw;
	const float ItemRotationYaw = GetActorRotation().Yaw;
	//Initial Yaw offset between camera and the item
	InterpInitialYawOffset = ItemRotationYaw - CameraRotationYaw;

	PlayPickupSound(bForcePlaySound);

	bCanChangeCustomDepth = false;
}


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Character/CharacterTypes.h"
#include "Engine/DataTable.h"
#include "Item.generated.h"

class UBoxComponent;
class UWidgetComponent;
class USphereComponent;
class AShooterCharacter;
class UCurveFloat;
class USoundBase;
class UCurveVector;
class UDataTable;

USTRUCT(BlueprintType)
struct FItemRarityTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FLinearColor GlowColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor LightColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor DarkColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumberOfStars;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* IconBackground;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CustomDepthStencil;
};

UCLASS()
class SHOOTER_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();
	virtual void Tick(float DeltaTime) override;
	void StartItemCurve(AShooterCharacter* Character, bool bForcePlaySound = false);
	void PlayEquipSound(bool bForcePlaySound = false);
	virtual void EnableCustomDepth();
	virtual void DisableCustomDepth();
	void EnableGlowMaterial();
	void DisableGlowMaterial();

protected:

	//Item Rarity Data Table
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Datatable")
	UDataTable* ItemRariryDataTable;

	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	void CurrentActiveStars(int32 TotalStars);

	void SetActiveStars();
	virtual void SetItemProperties(EItemState State);

	void ItemInterp(float DeltaTime);
	void FinishInterping();

	void PlayPickupSound(bool bForcePlaySound = false);
	void ResetPulseTimer();
	void StartPulseTimer();
	void UpdatePulse();
	virtual void InitializeCustomDepth();
	
	UPROPERTY(EditAnywhere, Category = "Item Properties")
	USkeletalMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, Category = "Item Properties")
	UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	int32 ItemCount;

	UPROPERTY(EditAnywhere, Category = "Item Properties")
	USphereComponent* AreaSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	EBulletRarity WeaponRarity = EBulletRarity::EBR_Common;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Item Properties")
	TArray<bool> ActiveStars;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties")
	EItemState ItemState = EItemState::EIS_Pickup;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	EItemType ItemType = EItemType::EIT_MAX;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	USoundBase* PickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	USoundBase* EquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	int32 MaterialIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties")
	UMaterialInstanceDynamic* DynamicMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	UMaterialInstance* MaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	UTexture2D* IconItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	UTexture2D* IconAmmo;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 SlotIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	bool bCharacterInventoryFull = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RarityDataTable")
	FLinearColor GlowColor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RarityDataTable")
	FLinearColor LightColor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RarityDataTable")
	FLinearColor DarkColor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RarityDataTable")
	int32 NumberOfStars;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RarityDataTable")
	UTexture2D* IconBackground;

private:

	FTimerHandle ItemInterpTimer;
	FTimerHandle PulseTimer;

	float ItemInterpX = 0.f;
	float ItemInterpY = 0.f;

	float InterpInitialYawOffset = 0.f;

	bool bCanChangeCustomDepth = true;

	UPROPERTY(EditDefaultsOnly, Category = "Item Properties")
	float PulseCurveTime = 5.f;

	UPROPERTY(VisibleAnywhere, Category = "Item Properties")
	float GlowAmount = 150.f;

	UPROPERTY(VisibleAnywhere, Category = "Item Properties")
	float FresnelExponent = 3.f;

	UPROPERTY(VisibleAnywhere, Category = "Item Properties")
	float FresnelReflectFraction = 4.f;

	UPROPERTY(EditDefaultsOnly, Category = "Item Properties")
	float ZCurveTime = 0.7f;

	UPROPERTY(VisibleAnywhere, Category = "Item Properties")
	AShooterCharacter* PlayerCharacter;

	UPROPERTY(VisibleAnywhere, Category = "Item Properties")
	FVector ItemInterpStartLocation = FVector(0.f);

	UPROPERTY(VisibleAnywhere, Category = "Item Properties")
	FVector CameraTargetLocation = FVector(0.f);

	UPROPERTY(VisibleAnywhere, Category = "Item Properties")
	bool bInterping = false;

	UPROPERTY(EditDefaultsOnly, Category = "Item Properties")
	UCurveFloat* ItemZCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Item Properties")
	UCurveFloat* ItemScaleCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Item Properties")
	UCurveVector* PulseCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Item Properties")
	UCurveVector* InterpPulseCurve;

	//Index of the interp location this item is interping to
	UPROPERTY(VisibleAnywhere, Category = "Item Properties")
	int32 InterpLocIndex = 0;

	

public:
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; };
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; };
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; };
	FORCEINLINE EItemState GetItemState() const { return ItemState; };
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; };
	FORCEINLINE USoundBase* GetPickupSound() const { return PickupSound; };
	FORCEINLINE USoundBase* GetEquipSound() const { return EquipSound; };
	FORCEINLINE int32 GetItemCount() const { return ItemCount; };
	FORCEINLINE int32 GetSlotIndex() const { return SlotIndex; };
	FORCEINLINE void SetSlotIndex(int32 Index) { SlotIndex = Index; };
	FORCEINLINE void SetCharacter(AShooterCharacter* Character) { PlayerCharacter = Character; };
	FORCEINLINE void SetCharacterInventoryFull(bool bFull) { bCharacterInventoryFull = bFull; };
	
	//Get InterpLocation based on the item type
	FVector GetInterpLocation();

	void SetITemState(EItemState State);
};

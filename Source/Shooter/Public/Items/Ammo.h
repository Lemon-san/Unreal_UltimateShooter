// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Character/CharacterTypes.h"
#include "Ammo.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API AAmmo : public AItem
{
	GENERATED_BODY()
	
public:
	AAmmo();
	virtual void Tick(float DeltaTime) override;
	virtual void EnableCustomDepth() override;
	virtual void DisableCustomDepth() override;

protected:

	virtual void BeginPlay() override;

	virtual void SetItemProperties(EItemState State) override;

	

	UFUNCTION()
	virtual void OnAmmoSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Ammo Properties")
	EAmmoType AmmoType;

	UPROPERTY(VisibleAnywhere, Category = "Ammo Properties")
	UStaticMeshComponent* AmmoMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo Properties")
	UTexture2D* AmmoIconTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo Properties")
	USphereComponent* AmmoCollisionSphere;

private:


public:

	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() const { return AmmoMesh; };
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; };
};

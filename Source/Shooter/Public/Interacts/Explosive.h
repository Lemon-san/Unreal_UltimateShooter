// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/BulletHitinterface.h"
#include "Explosive.generated.h"

class UParticleSystem;
class USoundBase;
class UCapsuleComponent;
class USphereComponent;

UCLASS()
class SHOOTER_API AExplosive : public AActor, public IBulletHitinterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosive();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ControllerInstigator) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USphereComponent* OverlapSphere;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Explosion")
	UParticleSystem* ExplosionParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	USoundBase* ExplosionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float ExplosionDamage = 85.f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

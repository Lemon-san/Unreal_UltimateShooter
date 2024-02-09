// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/BulletHitinterface.h"
#include "Enemy.generated.h"


class UParticleSystem;
class USoundBase;
class UWidgetComponent;
class UBehaviorTree;
class AEnemyAIController;
class USphereComponent;
class UBoxComponent;
class AShooterCharacter;

UCLASS()
class SHOOTER_API AEnemy : public ACharacter, public IBulletHitinterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowHitNumber(int32 Damage, FVector HitLocation, bool bHeadShot);

protected:

	virtual void BeginPlay() override;
	virtual void BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ControllerInstigator) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void SpawnCharacterHitParticles(FName WeaponSocket, AShooterCharacter* ShooterCharacter);
	void DoDamage(AShooterCharacter* OtherActor);
	void ResetHitReactTimer();
	void ShowHealthBar_Implementation();
	void UpdateHitNumbers();
	void Die();
	void PlayMontage(UAnimMontage* Montage, const FName& SectionName);
	void PlayHitMontage(UAnimMontage* Montage, const FName& SectionName);
	void AttemptStunCharacter(AShooterCharacter* Character);
	void ResetCanAttack();

	//UFUNCTION(BlueprintCallable);
	void DestroyEnemy();

	bool bCanHitReact = true;
	bool bDying = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DeathTime = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FTimerHandle DeathTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FTimerHandle AttackWaitTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bCanAttack = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackWaitTime = 1.f;

	UFUNCTION(BlueprintCallable)
	void FinishDeath();

	UFUNCTION(BlueprintCallable)
	void ActivateRightWeapon();

	UFUNCTION(BlueprintCallable)
	void DeactivateRightWeapon();

	UFUNCTION(BlueprintCallable)
	void ActivateLeftWeapon();

	UFUNCTION(BlueprintCallable)
	void DeactivateLeftWeapon();

	UFUNCTION()
	virtual void OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void EndCombatRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void CombatRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void SetStunned(bool Stunned);

	UFUNCTION()
	virtual void AggroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintNativeEvent)
	void ShowHealthBar();

	UFUNCTION()
	void DestroyHitNumber(UUserWidget* HitNumber);

	UFUNCTION(BlueprintImplementableEvent)
	void HideHealthBar();

	UFUNCTION(BlueprintCallable)
	void PlayAttackMontage(UAnimMontage* Montage, const FName& SectionName);

	UFUNCTION(BlueprintPure)
	FName GetAttackSectionName();

	UFUNCTION(BlueprintCallable)
	void StoreHitNumber(UUserWidget* HitNumber, FVector Location);

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	TMap<UUserWidget*, FVector> HitNumbers;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bStunned = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float StunChance = 0.5f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Combat")
	UParticleSystem* ImpactParticles;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	USoundBase* ImpactSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	float Health = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FString HeadBone = TEXT("head");

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widget")
	UWidgetComponent* HealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float HealthBarDisplayTime = 4.f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FTimerHandle HealthBarTimer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FTimerHandle HitReactTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float HitNumberDestroyTime = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (MakeEditWidget = "true"))
	USphereComponent* AggroSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	USphereComponent* CombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	 UBoxComponent* RightWeaponCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	 UBoxComponent* LeftWeaponCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bInAttackRange;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* HitMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montages", meta = (MakeEditWidget = "true"))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Montages")
	TArray<FName> MontageDeaths;
	
	//point for the enemy to move to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviorTree", meta = (MakeEditWidget = "true"))
	FVector PatrolPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviorTree", meta = (MakeEditWidget = "true"))
	FVector PatrolPoint2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BehaviorTree", meta = (MakeEditWidget = "true"))
	FVector PatrolPoint3;

	AEnemyAIController* EnemyAIController;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Montages")
	TArray<FName> AttackFasts;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Montages")
	TArray<FName> Attacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float BaseDamage = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FName LeftWeaponSocket = TEXT("FX_Trail_01_L");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FName RightWeaponSocket = TEXT("FX_Trail_01_R");

private:

	UPROPERTY(EditAnywhere, Category = "BehaviorTree")
	UBehaviorTree* EnemyBehaviorTree;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE FString GetHeadBone() { return HeadBone; };
	FORCEINLINE UBehaviorTree* GetBehaviorTree() { return EnemyBehaviorTree; };

};

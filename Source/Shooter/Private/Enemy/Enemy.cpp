// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Particles/ParticleSystemComponent.h"
#include <Components/WidgetComponent.h>
#include "Kismet/KismetMathLibrary.h"
#include "Shooter/DebugMacros.h"
#include "AI/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Character/ShooterCharacter.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Character/ShooterCharacter.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("Health Bar"));
	HealthBar->SetupAttachment(GetRootComponent());

	AggroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AggroSphereComponent"));
	AggroSphere->SetupAttachment(GetRootComponent());

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphereComponent"));
	CombatSphere->SetupAttachment(GetRootComponent());

	RightWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightWeaponBox"));
	RightWeaponCollision->SetupAttachment(GetMesh(), FName("RightWeaponBone"));

	LeftWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWeaponBox"));
	LeftWeaponCollision->SetupAttachment(GetMesh(), FName("LeftWeaponBone"));

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	//Delegate Dynamic Binds
	AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AggroSphereOverlap);
	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatRangeOverlap);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::EndCombatRangeOverlap);
	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnRightWeaponOverlap);
	LeftWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnLeftWeaponOverlap);

	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility,ECollisionResponse::ECR_Block);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	EnemyAIController = Cast<AEnemyAIController>(GetController());
	if (!EnemyAIController) return;

	if (EnemyAIController)
	{
		EnemyAIController->GetBlackBoardComponent()->SetValueAsBool(FName("CanAttack"), true);
	}

	const FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint);
	const FVector WorldPatrolPoint2 = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint2);
	const FVector WorldPatrolPoint3 = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint3);

	EnemyAIController->GetBlackBoardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WorldPatrolPoint);
	EnemyAIController->GetBlackBoardComponent()->SetValueAsVector(TEXT("PatrolPoint2"), WorldPatrolPoint2);
	EnemyAIController->GetBlackBoardComponent()->SetValueAsVector(TEXT("PatrolPoint3"), WorldPatrolPoint3);

	EnemyAIController->RunBehaviorTree(EnemyBehaviorTree);

}

void AEnemy::BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ControllerInstigator)
{
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());

	}

	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, HitResult.Location, FRotator(0.f), true);

	}


	
}

void AEnemy::ResetHitReactTimer()
{
	bCanHitReact = true;
}

void AEnemy::FinishDeath()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::DestroyEnemy, DeathTime);
}

void AEnemy::ActivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::ActivateLeftWeapon()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateLeftWeapon()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
	if (!ShooterCharacter) return;
	DoDamage(ShooterCharacter);
	SpawnCharacterHitParticles(RightWeaponSocket, ShooterCharacter);
	AttemptStunCharacter(ShooterCharacter);
}

void AEnemy::SpawnCharacterHitParticles(FName WeaponSocket, AShooterCharacter* ShooterCharacter)
{
	const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName(WeaponSocket);
	if (TipSocket && ShooterCharacter->GetBloodParticles())
	{
		const FTransform SocketTransform = TipSocket->GetSocketTransform(GetMesh());
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ShooterCharacter->GetBloodParticles(), SocketTransform);
	}
}
void AEnemy::DoDamage(AShooterCharacter* OtherActor)
{
	if (OtherActor && OtherActor->GetImpactSound())
	{
		UGameplayStatics::ApplyDamage(OtherActor, BaseDamage, EnemyAIController, this, UDamageType::StaticClass());
		UGameplayStatics::PlaySoundAtLocation(this, OtherActor->GetImpactSound(), GetActorLocation());
	}
}

void AEnemy::OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
	if (!ShooterCharacter) return;
	DoDamage(ShooterCharacter);
	SpawnCharacterHitParticles(LeftWeaponSocket, ShooterCharacter);
	AttemptStunCharacter(ShooterCharacter);
}

void AEnemy::EndCombatRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor) return;

	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);

	if (ShooterCharacter)
	{
		bInAttackRange = false;

		if (EnemyAIController)
		{
			EnemyAIController->GetBlackBoardComponent()->SetValueAsBool(TEXT("InAttackRange"), bInAttackRange);
		}
	}
	
}

void AEnemy::CombatRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor);

	if (ShooterCharacter)
	{
		bInAttackRange = true;

		if (EnemyAIController)
		{
			EnemyAIController->GetBlackBoardComponent()->SetValueAsBool(TEXT("InAttackRange"), bInAttackRange);
		}
	}
	
}

void AEnemy::SetStunned(bool Stunned)
{
	bStunned = Stunned;

	if (EnemyAIController)
	{
		EnemyAIController->GetBlackBoardComponent()->SetValueAsBool(TEXT("Stunned"),Stunned);
	}
}

void AEnemy::AggroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;
	AShooterCharacter* Character = Cast<AShooterCharacter>(OtherActor);

	if (Character)
	{
		if (!EnemyAIController) return;
		if (!EnemyAIController->GetBlackBoardComponent()) return;

		EnemyAIController->GetBlackBoardComponent()->SetValueAsObject(TEXT("Target"), Character);
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//Set The target blackboard key to agro the characgter
	if (EnemyAIController)
	{
		EnemyAIController->GetBlackBoardComponent()->SetValueAsObject(FName("Target"), DamageCauser);
	}

	if (Health - DamageAmount <= 0.f)
	{
		Health = 0;
		Die();
	}

	else
	{
		Health -= DamageAmount;
	}

	if (bDying == true) return DamageAmount;

	ShowHealthBar();

	const float RandomChanceStun = FMath::FRandRange(0.f, 1.f);
	if (RandomChanceStun <= StunChance)
	{
		//Stun Enemy
		PlayHitMontage(HitMontage, FName("HitFront"));
		SetStunned(true);
	}

	return DamageAmount;
}

void AEnemy::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	GetWorldTimerManager().SetTimer(HealthBarTimer, this, &AEnemy::HideHealthBar, HealthBarDisplayTime);
}

void AEnemy::UpdateHitNumbers()
{
	for (auto& HitPair : HitNumbers)
	{
		UUserWidget* HitNumber = HitPair.Key;
		const FVector Location = HitPair.Value;

		FVector2D ScreenPosition;
		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), Location, ScreenPosition);

		HitNumber->SetPositionInViewport(ScreenPosition);

	}
}

void AEnemy::DestroyHitNumber(UUserWidget* HitNumber)
{
	HitNumbers.Remove(HitNumber);
	HitNumber->RemoveFromParent();
}

void AEnemy::Die()
{
	if (bDying == true) return;
	bDying = true;

	HideHealthBar();

	const int DeathRandom = FMath::RandRange(0,MontageDeaths.Num() - 1);
	PlayMontage(DeathMontage, MontageDeaths[DeathRandom]);

	if (EnemyAIController)
	{
		EnemyAIController->GetBlackBoardComponent()->SetValueAsBool(FName("IsDead"), true);
		EnemyAIController->StopMovement();
	}
}

void AEnemy::PlayMontage(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AEnemy::PlayHitMontage(UAnimMontage* Montage, const FName& SectionName)
{
	
	PlayMontage(Montage, SectionName);

	if (bCanHitReact)
	{
		bCanHitReact = false;
		const float HitReactTime = FMath::RandRange(0.5f, 4.f);
		GetWorldTimerManager().SetTimer(HitReactTimer, this, &AEnemy::ResetHitReactTimer, HitReactTime);
	}
}

void AEnemy::AttemptStunCharacter(AShooterCharacter* Character)
{
	if (!Character) return;

	const float Stun = FMath::FRandRange(0.f, 1.f);
	if (Stun <= Character->GetStunChance())
	{
		Character->Stun();
	}
}

void AEnemy::ResetCanAttack()
{
	bCanAttack = true;
	if (EnemyAIController)
	{
		EnemyAIController->GetBlackBoardComponent()->SetValueAsBool(FName("CanAttack"), bCanAttack);
	}
}

void AEnemy::DestroyEnemy()
{
	Destroy();
}

void AEnemy::PlayAttackMontage(UAnimMontage* Montage, const FName& SectionName)
{
	PlayMontage(Montage, SectionName);
	bCanAttack = false;
	GetWorldTimerManager().SetTimer(AttackWaitTimer, this, &AEnemy::ResetCanAttack, AttackWaitTime);

	if (EnemyAIController)
	{
		EnemyAIController->GetBlackBoardComponent()->SetValueAsBool(FName("CanAttack"), bCanAttack);
	}
}

FName AEnemy::GetAttackSectionName()
{
	FName RandomAttack;
	const int AttackRandom = FMath::RandRange(0, Attacks.Num() - 1);
	const int AttackFastRandom = FMath::RandRange(0, AttackFasts.Num() - 1);
	const int SwitchAttack = FMath::RandRange(0, 1);

	switch (SwitchAttack)
	{
	case 0:
		RandomAttack = Attacks[AttackRandom];
		break;
	case 1:
		RandomAttack = AttackFasts[AttackFastRandom];
		break;
	}

	return RandomAttack;
}

void AEnemy::StoreHitNumber(UUserWidget* HitNumber, FVector Location)
{
	HitNumbers.Add(HitNumber, Location);

	FTimerHandle HitNumberTimer;
	FTimerDelegate HitNumberDelegate;
	HitNumberDelegate.BindUFunction(this, FName("DestroyHitNumber"), HitNumber);
	GetWorldTimerManager().SetTimer(HitNumberTimer, HitNumberDelegate, HitNumberDestroyTime, false);

	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateHitNumbers();

}



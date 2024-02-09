// Fill out your copyright notice in the Description page of Project Settings.


#include "Interacts/Explosive.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AExplosive::AExplosive()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	SetRootComponent(CapsuleComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	StaticMeshComponent->SetupAttachment(CapsuleComponent);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Overlap Component"));
	OverlapSphere->SetupAttachment(GetRootComponent());
	
}

// Called when the game starts or when spawned
void AExplosive::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExplosive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosive::BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ControllerInstigator)
{
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());

	}

	if (ExplosionParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticles, HitResult.Location, FRotator(0.f), true);

	}
	
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());

	for (AActor* Actor : OverlappingActors)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor Damaged by explosive: %s"), *Actor->GetName());
		UGameplayStatics::ApplyDamage(Actor, ExplosionDamage, ControllerInstigator, Shooter, UDamageType::StaticClass());
	}

	Destroy();
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAnimInstance.h"
#include "Enemy/Enemy.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (Enemy == nullptr)
	{
		Enemy = Cast<AEnemy>(TryGetPawnOwner());
	}

	if (Enemy)
	{

		FVector Velocity = Enemy->GetVelocity();
		Velocity.Z = 0.f;
		Speed = Velocity.Size();
	}
}

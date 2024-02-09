// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Enemy/Enemy.h"

AEnemyAIController::AEnemyAIController()
{
	BlackBoardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	check(BlackBoardComponent);

	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	check(BehaviorTreeComponent);


}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!InPawn) return;

	AEnemy* Enemy = Cast<AEnemy>(InPawn);
	if (!Enemy) return;

	if (Enemy->GetBehaviorTree())
	{
		BlackBoardComponent->InitializeBlackboard(*(Enemy->GetBehaviorTree()->BlackboardAsset));
	}
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class UBlackboardComponent;
class UBehaviorTreeComponent;
/**
 * 
 */
UCLASS()
class SHOOTER_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();
	virtual void OnPossess(APawn* InPawn) override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AIBehavior")
	UBlackboardComponent* BlackBoardComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AIBehavior")
	UBehaviorTreeComponent* BehaviorTreeComponent;

private:

public:

	FORCEINLINE UBlackboardComponent* GetBlackBoardComponent() { return BlackBoardComponent; };
	
};

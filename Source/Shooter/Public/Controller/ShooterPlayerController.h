// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

class UUserWidget;
/**
 * 
 */
UCLASS()
class SHOOTER_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AShooterPlayerController();


protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> HUDOverlayClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widgets")
	UUserWidget* HUDOverlay;

private:

	
	
};

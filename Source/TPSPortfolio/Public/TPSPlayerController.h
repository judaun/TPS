// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TPSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TPSPORTFOLIO_API ATPSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATPSPlayerController();
	~ATPSPlayerController();
public:
	/* Reference to the UMG asset in the editor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<UUserWidget> MainHUDWidgetClass;

	/* Variable to hold the widget after creating it */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		UUserWidget* CrossHairHUDWidget;

protected:
	virtual void BeginPlay() override;

protected:
	void Tick(float DeltaSeconds) override;
};

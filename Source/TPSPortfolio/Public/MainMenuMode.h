// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuMode.generated.h"

/**
 * 
 */
UCLASS()
class TPSPORTFOLIO_API AMainMenuMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMainMenuMode();
protected:
	virtual void BeginPlay() override;
private:
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	class UMainMenu* MainMenuWidget;
};

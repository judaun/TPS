// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LoadingMod.generated.h"

/**
 * 
 */
UCLASS()
class TPSPORTFOLIO_API ALoadingMod : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ALoadingMod();
protected:
	virtual void BeginPlay() override;
private:
	TSubclassOf<UUserWidget> LoadingWidgetClass;
	class UUserWidget* LoadingWidget;
};

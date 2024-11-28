// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class TPSPORTFOLIO_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()
	
private:
	

public:
	void NativeConstruct();
	UFUNCTION(BlueprintCallable)
	void Click_Main();
	UFUNCTION(BlueprintCallable)
	void Click_Test();

//value/////////////////////////////////////////////////
private:
	UPROPERTY()
	class UButton* Btt_Main;
	UPROPERTY()
	class UButton* Btt_Test;
protected:
public:
};

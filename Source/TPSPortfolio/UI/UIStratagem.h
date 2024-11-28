// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIStratagem.generated.h"

/**
 * 
 */

 class AStratagem;

UCLASS()
class TPSPORTFOLIO_API UUIStratagem : public UUserWidget
{
	GENERATED_BODY()

private:
	void InitializeComponent();

public:
	void NativeConstruct();
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	void SetTime(int32 idx, int32 status, float time);
	void SetSkillData(AStratagem* owner);
	void SetSkillStatus(FString skillstatus);
	void SetLocation(FVector pos) { vPos = pos;}
	void SetDistance();
	void UIActive(bool ison);
private:
	UPROPERTY()
		class UTextBlock* UTSkillName_txt;
	UPROPERTY()
		class UTextBlock* UTSkillStatus_txt;
	UPROPERTY()
		class UTextBlock* UTSkillDistance_txt;

	UPROPERTY()
		class UImage* UISkillIcon;

	FVector vPos;
	float fdistancetime = 0.f;
};

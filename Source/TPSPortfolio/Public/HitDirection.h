// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HitDirection.generated.h"

/**
 * 
 */
UCLASS()
class TPSPORTFOLIO_API UHitDirection : public UUserWidget
{
	GENERATED_BODY()
	
private:
	void SetAlphaDecrease(float deltatime);
protected:
	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
public:
	void SetActive(float degree);
	void SetActiveBlood(float maxalpha);
	void BindUserData(class ATPSPortfolioCharacter* character);
private:
	UPROPERTY()
	class UImage* UIHitDirection;

	UPROPERTY()
	class UImage* UIBlood;

	FLinearColor HitColor;
	FLinearColor BloodColor;
	float fMaxAlpha;
	bool bIsBlood;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "TPSPortfolioCharacter.h"
#include "CrossHair.generated.h"
/**
 * 
 */

UCLASS()
class TPSPORTFOLIO_API UCrossHair : public UUserWidget
{
	GENERATED_BODY()
public:
	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
public:
	void AimRate(float Aimrate);
	void SetAimRate(float Aimrate);
	void SetBulletProgress(float remainpercent);

	void BindUserAimRate(ATPSPortfolioCharacter* TPSCharacter);
private:
	UImage* UIBullet_Progress;
	UImage* UICross_L;
	UImage* UICross_R;
	UImage* UICross_T;
	UImage* UICross_B;

	UMaterialInstanceDynamic* pMaterial;

	float fAimrate;
	float fTarget_Aimrate;
};

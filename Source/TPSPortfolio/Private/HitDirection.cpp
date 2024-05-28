// Fill out your copyright notice in the Description page of Project Settings.


#include "HitDirection.h"
#include "Components/Image.h"
#include "TPSPortfolioCharacter.h"

void UHitDirection::SetAlphaDecrease(float deltatime)
{
	if (!IsValid(UIHitDirection)) return;
	
	if(HitColor.A > 0.f)
		HitColor.A -= deltatime;

	UIHitDirection->SetColorAndOpacity(HitColor);

	if(!IsValid(UIBlood)) return;

	//(sin(x)+1) / 2.f = 0~1 นÝบน
	BloodColor.A = bIsBlood ? (sin(GetWorld()->GetTimeSeconds()*3.f)+1) / fMaxAlpha : 0.f;
	UIBlood->SetColorAndOpacity(BloodColor);
}

void UHitDirection::NativeConstruct()
{
	Super::NativeConstruct();

	UIHitDirection = Cast<UImage>(GetWidgetFromName(TEXT("HitMark")));
	UIBlood = Cast<UImage>(GetWidgetFromName(TEXT("Blood")));

	HitColor = FLinearColor(1.f,1.f,1.f,0.f);
	BloodColor = FLinearColor(1.f, 1.f, 1.f, 0.f);
	fMaxAlpha = 2.f;
}

void UHitDirection::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	SetAlphaDecrease(InDeltaTime);
}

void UHitDirection::SetActive(float degree)
{
	if(!IsValid(UIHitDirection)) return;

	UIHitDirection->SetRenderTransformAngle(degree);
	
	HitColor.A = 0.7f;
	UIHitDirection->SetColorAndOpacity(HitColor);
}

void UHitDirection::SetActiveBlood(float maxalpha)
{
	//ZeroDivide
	if(maxalpha <= 0.f || maxalpha > 0.5f)
	{
		bIsBlood = false;
		return;
	}
	else
	{
		bIsBlood = true;

		fMaxAlpha = maxalpha*4 +2;
	}
}

void UHitDirection::BindUserData(class ATPSPortfolioCharacter* character)
{
	if(!IsValid(character)) return;

	character->func_Player_HP.AddUObject(this, &UHitDirection::SetActiveBlood);
}

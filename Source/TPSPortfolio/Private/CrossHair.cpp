// Fill out your copyright notice in the Description page of Project Settings.


#include "CrossHair.h"

void UCrossHair::NativeConstruct()
{
	Super::NativeConstruct();
	UICross_L = Cast<UImage>(GetWidgetFromName(TEXT("Cross_L")));
	UICross_R = Cast<UImage>(GetWidgetFromName(TEXT("Cross_R")));
	UICross_T = Cast<UImage>(GetWidgetFromName(TEXT("Cross_T")));
	UICross_B = Cast<UImage>(GetWidgetFromName(TEXT("Cross_B")));

	fAimrate = 0.f;
	fTarget_Aimrate = 1.f;
}

void UCrossHair::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (fTarget_Aimrate != fAimrate) AimRate(InDeltaTime);
}


void UCrossHair::AimRate(float InDeltaTime)
{
	if (!IsValid(UICross_L)
		|| !IsValid(UICross_R)
		|| !IsValid(UICross_T)
		|| !IsValid(UICross_B))
		return;

	
	fAimrate = FMath::FInterpTo(fAimrate, fTarget_Aimrate, InDeltaTime, 5.f);
	if (abs(fTarget_Aimrate - fAimrate) < FLT_EPSILON)
		fAimrate = fTarget_Aimrate;

	UICross_L->SetRenderTranslation(FVector2D(-16.f * fAimrate, 0.f));
	UICross_R->SetRenderTranslation(FVector2D(16.f * fAimrate, 0.f));
	UICross_T->SetRenderTranslation(FVector2D(0.f, -16.f * fAimrate));
	UICross_B->SetRenderTranslation(FVector2D(0.f, 16.f * fAimrate));
}

void UCrossHair::SetAimRate(float Aimrate)
{
	fTarget_Aimrate = Aimrate;
}

void UCrossHair::BindUserAimRate(ATPSPortfolioCharacter* TPSCharacter)
{
	if(TPSCharacter == nullptr) return;
	TPSCharacter->func_Player_Aimrate.BindUObject(this, &UCrossHair::SetAimRate);
}

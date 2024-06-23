// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterHUD.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "TPSPortfolioCharacter.h"
#include "Components/Image.h"

#define LOCTEXT_NAMESPACE "TPSNameSpace"

void UCharacterHUD::NativeConstruct()
{
	Super::NativeConstruct();

	//UIBullet_Progress = Cast<UImage>(GetWidgetFromName(TEXT("Bullet_Progress")));
	UTGrenade_txt = Cast<UTextBlock>(GetWidgetFromName(TEXT("Grenade_txt")));
	UTHealBox_txt = Cast<UTextBlock>(GetWidgetFromName(TEXT("HealBox_txt")));
	UTAmmo_txt = Cast<UTextBlock>(GetWidgetFromName(TEXT("Ammo_txt")));
	UTMagazine_txt = Cast<UTextBlock>(GetWidgetFromName(TEXT("Magazine_txt")));

	UPHealth = Cast<UProgressBar>(GetWidgetFromName(TEXT("Health_Gauge")));

	srcColor = {0.15f,1.f,0.15f};
	dstColor = {1.f, 0.15f, 0.15f};
}

void UCharacterHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UCharacterHUD::SetAmmo(int32 ammocnt, int32 maxammo)
{
	if (!IsValid(UTAmmo_txt)) return;

	FText text = FText::Format(LOCTEXT("UI_Character_Ammo","{0} / {1}"), ammocnt, maxammo);
	UTAmmo_txt->SetText(text);
}

void UCharacterHUD::SetGrenade(int32 grenadecnt)
{
	if (!IsValid(UTGrenade_txt)) return;

	FText text = FText::Format(LOCTEXT("UI_Character_Grenade", "x{0}"), grenadecnt);
	UTGrenade_txt->SetText(text);
}

void UCharacterHUD::SetHealBox(int32 healboxcnt)
{
	if (!IsValid(UTHealBox_txt)) return;

	FText text = FText::Format(LOCTEXT("UI_Character_Heal", "x{0}"), healboxcnt);
	UTHealBox_txt->SetText(text);
}

void UCharacterHUD::SetMagazine(int32 magazinecnt)
{
	if (!IsValid(UTMagazine_txt)) return;

	FText text = FText::Format(LOCTEXT("UI_Character_Magazine", "x{0}"), magazinecnt);
	UTMagazine_txt->SetText(text);
}

void UCharacterHUD::SetHealthGauge(float healthrate)
{
	if (!IsValid(UPHealth)) return;

	UPHealth->SetPercent(healthrate);
	
	FVector vLerp = FMath::Lerp(dstColor, srcColor,healthrate);
	FLinearColor color = FLinearColor(vLerp.X,vLerp.Y,vLerp.Z,1.f);
	UPHealth->SetFillColorAndOpacity(color);
}

void UCharacterHUD::BindUserData(ATPSPortfolioCharacter* tpscharacter)
{
	if (!IsValid(tpscharacter)) return;

	tpscharacter->func_Player_Bullet.AddUObject(this, &UCharacterHUD::SetAmmo);
	tpscharacter->func_Player_Magazine.BindUObject(this, &UCharacterHUD::SetMagazine);
	tpscharacter->func_Player_HealBox.BindUObject(this, &UCharacterHUD::SetHealBox);
	tpscharacter->func_Player_HP.AddUObject(this,&UCharacterHUD::SetHealthGauge);
	tpscharacter->func_Player_Grenade.BindUObject(this, &UCharacterHUD::SetGrenade);
}

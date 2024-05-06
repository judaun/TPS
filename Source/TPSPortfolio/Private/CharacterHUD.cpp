// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterHUD.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "TPSPortfolioCharacter.h"

#define LOCTEXT_NAMESPACE "TPSNameSpace"

void UCharacterHUD::NativeConstruct()
{
	Super::NativeConstruct();

	//UIBullet_Progress = Cast<UImage>(GetWidgetFromName(TEXT("Bullet_Progress")));
	UTGrenade_txt = Cast<UTextBlock>(GetWidgetFromName(TEXT("Grenade_txt")));
	UTHealBox_txt = Cast<UTextBlock>(GetWidgetFromName(TEXT("HealBox_txt")));
	UTAmmo_txt = Cast<UTextBlock>(GetWidgetFromName(TEXT("Ammo_txt")));
	UTMagazine_txt = Cast<UTextBlock>(GetWidgetFromName(TEXT("Magazine_txt")));

	//stamina_gauge Health_Gauge
	UPStamina = Cast<UProgressBar>(GetWidgetFromName(TEXT("stamina_gauge")));
	UPHealth = Cast<UProgressBar>(GetWidgetFromName(TEXT("Health_Gauge")));
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
}

void UCharacterHUD::SetHealBox(int32 healboxcnt)
{
	if (!IsValid(UTHealBox_txt)) return;

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

}

void UCharacterHUD::SetStaminaGauge(float staminarate)
{
	if (!IsValid(UPStamina)) return;

}

void UCharacterHUD::BindUserData(ATPSPortfolioCharacter* tpscharacter)
{
	if (!IsValid(tpscharacter)) return;

	tpscharacter->func_Player_Bullet.AddUObject(this, &UCharacterHUD::SetAmmo);
	tpscharacter->func_Player_Magazine.BindUObject(this, &UCharacterHUD::SetMagazine);
}

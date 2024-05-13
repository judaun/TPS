// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterHUD.generated.h"

/**
 * 
 */

class UTextBlock;
class UProgressBar;
class ATPSPortfolioCharacter;

UCLASS()
class TPSPORTFOLIO_API UCharacterHUD : public UUserWidget
{
	GENERATED_BODY()
		//Grenade_txt
		//HealBox_txt
		//Ammo_txt
		//Magazine_txt
public:
		void NativeConstruct();
		void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
		void SetAmmo(int32 ammocnt, int32 maxammo);
		void SetGrenade(int32 grenadecnt);
		void SetHealBox(int32 healboxcnt);
		void SetMagazine(int32 magazinecnt);

		void SetHealthGauge(float healthrate);
		void SetStaminaGauge(float staminarate);

		void BindUserData(ATPSPortfolioCharacter* tpscharacter);
private:
	UPROPERTY()
	UTextBlock* UTGrenade_txt;
	UPROPERTY()
	UTextBlock* UTHealBox_txt;
	UPROPERTY()
	UTextBlock* UTAmmo_txt;
	UPROPERTY()
	UTextBlock* UTMagazine_txt;

	UPROPERTY()
	UProgressBar* UPHealth;
	UPROPERTY()
	UProgressBar* UPStamina;
};

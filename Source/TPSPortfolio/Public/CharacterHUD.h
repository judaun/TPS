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

USTRUCT()
struct FSkillCommand 
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY()
		class UCanvasPanel* SkillPanel_0;
	UPROPERTY()
		UTextBlock* SkillCooldown_txt_0;
	UPROPERTY()
		class UImage* SkillIcon_0;
	UPROPERTY()
		UTextBlock* SkillName_txt_0;
	UPROPERTY()
		class UHorizontalBox* SkillCommand_Box_0;
	UPROPERTY()
		TArray<class UImage*> SkillCommand_0;
};

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

		void BindUserData(ATPSPortfolioCharacter* tpscharacter);

		void ChangeWeapon(int32 index);
		void SetWeapon(int32 number, FString weaponname);

		void SetSkillData(int32 idx, FString skillname, FString icon, int32 command);
		void CommandFlag(bool ison) {isOnCommand = ison;}
		void CommandOpen(float InDeltaTime);
		void SkillCooldown(int32 idx, float remaintime);
		/* 커맨드 체크 후 활성될 스킬아이콘 idx:스킬 인덱스 activecnt: 활성화할 수 */
		void CommandCheck(int32 idx, int32 activecnt, bool ison);
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
	class UImage* Portrait_Main;
	UPROPERTY()
	class UImage* Portrait1;
	UPROPERTY()
	class UImage* Portrait2;
	UPROPERTY()
	class UImage* Portrait3;

	UPROPERTY()
	UTextBlock* UTPortrait_txt_1;
	UPROPERTY()
	UTextBlock* UTPortrait_txt_2;
	UPROPERTY()
	UTextBlock* UTPortrait_txt_3;

	UPROPERTY()
	UProgressBar* UPHealth;

	FVector srcColor;
	FVector dstColor;

#pragma region SkillCommand
	UPROPERTY()
		TArray<FSkillCommand> USkillCommand;

	UPROPERTY()
		class UCanvasPanel* SkillPanel_Main;
	bool isOnCommand = false;
	float fCommandAlign = 1.f;
	float fPanelSizeX = 0.f;
	FWidgetTransform transSkillPanel;
#pragma endregion SkillCommand
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterHUD.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "TPSPortfolioCharacter.h"
#include "Stratagem.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/HorizontalBox.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateColor.h"
#include "Styling/SlateBrush.h"
#include "Widgets/Layout/SConstraintCanvas.h"


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

	Portrait_Main = Cast<UImage>(GetWidgetFromName(TEXT("Portrait_MW")));
	Portrait1 = Cast<UImage>(GetWidgetFromName(TEXT("Portrait_1")));
	Portrait2 = Cast<UImage>(GetWidgetFromName(TEXT("Portrait_2")));
	Portrait3 = Cast<UImage>(GetWidgetFromName(TEXT("Portrait_3")));

	UTPortrait_txt_1 = Cast<UTextBlock>(GetWidgetFromName(TEXT("Portrait_Text_1")));
	UTPortrait_txt_2 = Cast<UTextBlock>(GetWidgetFromName(TEXT("Portrait_Text_2")));
	UTPortrait_txt_3 = Cast<UTextBlock>(GetWidgetFromName(TEXT("Portrait_Text_3")));

	srcColor = {0.15f,1.f,0.15f};
	dstColor = {1.f, 0.15f, 0.15f};

	//SkillPanel_0 : 스킬커맨드창
	//Command_0_CoolDown : 스킬커맨드 쿨타임
	//SKillImg_0 : 스킬 아이콘
	//SkillText_0 : 스킬 이름
	//Command_0 : horizonalbox 스킬커맨드
	//Command_0_0 : 스킬 커맨드 아이콘
	SkillPanel_Main = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("SkillPanel")));
	SkillPanel_Main->ForceLayoutPrepass();
	fPanelSizeX = SkillPanel_Main->GetDesiredSize().X;
	transSkillPanel = SkillPanel_Main->GetRenderTransform();

	for (int i = 0; i < 3; ++i)
	{
		FSkillCommand newCommand;

		newCommand.SkillPanel_0 = Cast<UCanvasPanel>(GetWidgetFromName(*FString::Printf(TEXT("SkillPanel_%d"), i)));
		newCommand.SkillCooldown_txt_0 = Cast<UTextBlock>(GetWidgetFromName(*FString::Printf(TEXT("Command_%d_CoolDown"), i)));
		newCommand.SkillIcon_0 = Cast<UImage>(GetWidgetFromName(*FString::Printf(TEXT("SKillImg_%d"), i)));
		newCommand.SkillName_txt_0 = Cast<UTextBlock>(GetWidgetFromName(*FString::Printf(TEXT("SkillText_%d"), i)));
		newCommand.SkillCommand_Box_0 = Cast<UHorizontalBox>(GetWidgetFromName(*FString::Printf(TEXT("Command_%d"), i)));

		for (int j = 0; j < 5; ++j)
		{
			UImage* pImage = Cast<UImage>(GetWidgetFromName(*FString::Printf(TEXT("Command_%d_%d"), i,j)));
			newCommand.SkillCommand_0.Emplace(pImage);
		}

		USkillCommand.Emplace(newCommand);
	}
}

void UCharacterHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	CommandOpen(InDeltaTime);
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
	tpscharacter->func_Player_SkillCool.BindUObject(this, &UCharacterHUD::SkillCooldown);
	tpscharacter->func_Player_Coomandflag.BindUObject(this, &UCharacterHUD::CommandFlag);
}

void UCharacterHUD::ChangeWeapon(int32 index)
{
	UImage* pImage = nullptr;
	UTextBlock* pText = nullptr;

	switch (index)
	{
	case 1:
		pImage = Portrait1;
		pText = UTPortrait_txt_1;

		if (!UTPortrait_txt_2->GetText().IsEmpty())
		{
			Portrait2->SetColorAndOpacity(FLinearColor(FColor(255, 255, 255, 77)));
			UTPortrait_txt_2->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 1.f, 1.f, 0.3f)));
		}
		if (!UTPortrait_txt_3->GetText().IsEmpty())
		{
			Portrait3->SetColorAndOpacity(FLinearColor(FColor(255, 255, 255, 77)));
			UTPortrait_txt_3->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 1.f, 1.f, 0.3f)));
		}
		break;
	case 2:
		pImage = Portrait2;
		pText = UTPortrait_txt_2;

		if (!UTPortrait_txt_1->GetText().IsEmpty())
		{
			Portrait1->SetColorAndOpacity(FLinearColor(FColor(255, 255, 255, 77)));
			UTPortrait_txt_1->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 1.f, 1.f, 0.3f)));
		}
		if (!UTPortrait_txt_3->GetText().IsEmpty())
		{
			Portrait3->SetColorAndOpacity(FLinearColor(FColor(255, 255, 255, 77)));
			UTPortrait_txt_3->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 1.f, 1.f, 0.3f)));
		}
		break;
	case 3:
		pImage = Portrait3;
		pText = UTPortrait_txt_3;

		if (!UTPortrait_txt_1->GetText().IsEmpty())
		{
			Portrait1->SetColorAndOpacity(FLinearColor(FColor(255, 255, 255, 77)));
			UTPortrait_txt_1->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 1.f, 1.f, 0.3f)));
		}
		if (!UTPortrait_txt_2->GetText().IsEmpty())
		{
			Portrait2->SetColorAndOpacity(FLinearColor(FColor(255, 255, 255, 77)));
			UTPortrait_txt_2->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 1.f, 1.f, 0.3f)));
		}
		break;
	}

	if(!IsValid(pImage) || !IsValid(pText)) return;

	pImage->SetColorAndOpacity(FLinearColor(FColor(255,255,255,255)));
	pText->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 1.f, 1.f, 1.f)));

	UTexture2D* pTexture = Cast<UTexture2D>(pImage->GetBrush().GetResourceObject());
	Portrait_Main->SetBrushFromTexture(pTexture);

}

void UCharacterHUD::SetWeapon(int32 number, FString weaponname)
{
	UImage* pImage = nullptr;
	UTextBlock* pText = nullptr;

	switch (number)
	{
	 case 1: 
		 pImage = Portrait1;
		 pText = UTPortrait_txt_1;
	 break;
	 case 2: 
		 pImage = Portrait2;
		 pText = UTPortrait_txt_2;
	 break;
	 case 3: 
		 pImage = Portrait3;
		 pText = UTPortrait_txt_3;
	 break;
	}

	if(!IsValid(pImage) || !IsValid(pText)) return;

	FString textureaddress = FString::Printf(TEXT("Script/Engine.Texture2D'/Game/UI/Textures/%s.%s'"),  *weaponname, *weaponname);
	UTexture2D* pTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *textureaddress));
	FText text = FText::FromString(weaponname);
	pImage->SetBrushFromTexture(pTexture);
	pText->SetText(text);

	pImage->SetColorAndOpacity(FLinearColor(FColor(255, 255, 255, 77)));
	pText->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 1.f, 1.f, 0.3f)));
}

void UCharacterHUD::SetSkillData(int32 idx, FString skillname, FString icon, int32 command)
{
	
	USkillCommand[idx].SkillName_txt_0->SetText(FText::FromString(*skillname));

	FString textureaddress = FString::Printf(TEXT("Script/Engine.Texture2D'/Game/UI/Textures/%s.%s'"), *skillname, *skillname);
	UTexture2D* pTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *textureaddress));
	USkillCommand[idx].SkillIcon_0->SetBrushFromTexture(pTexture);

	FString strcommand = FString::FromInt(command);
	for (size_t i=0; i<strcommand.Len(); ++i)
	{
		//1 < 2^ 3v 
		switch (strcommand[i] - '0')
		{
			case 1:
				USkillCommand[idx].SkillCommand_0[i]->SetRenderTransformAngle(180.f);
			break;
			case 2:
				USkillCommand[idx].SkillCommand_0[i]->SetRenderTransformAngle(-90.f);
			break;
			case 3:
				USkillCommand[idx].SkillCommand_0[i]->SetRenderTransformAngle(90.f);
			break;
			default:
			break;
		}
	}

}

void UCharacterHUD::CommandOpen(float InDeltaTime)
{
	if(isOnCommand && fCommandAlign > 0.f) 
	{ 
	fCommandAlign -= InDeltaTime * 3.f;

	FWidgetTransform newtrans = transSkillPanel;
	newtrans.Translation.X += fPanelSizeX *(1.f-fCommandAlign);
	SkillPanel_Main->SetRenderTransform(newtrans);
	}
	else if(!isOnCommand && fCommandAlign < 1.f) 
	{
	fCommandAlign += InDeltaTime * 3.f;

	FWidgetTransform newtrans = transSkillPanel;
	newtrans.Translation.X += fPanelSizeX * (1.f - fCommandAlign);
	SkillPanel_Main->SetRenderTransform(newtrans);
	}
}

void UCharacterHUD::SkillCooldown(int32 idx, float time)
{
	if (time > 0.f)
	{
		USkillCommand[idx].SkillCommand_Box_0->SetRenderOpacity(0.f);
		USkillCommand[idx].SkillCooldown_txt_0->SetRenderOpacity(1.f);
	}
	else
	{
		USkillCommand[idx].SkillCommand_Box_0->SetRenderOpacity(1.f);
		USkillCommand[idx].SkillCooldown_txt_0->SetRenderOpacity(0.f);
	}

	FText ftxt = FText::Format(LOCTEXT("UI_Statagem_CoolDown", "Cooldown : {0}"), round(time));
	USkillCommand[idx].SkillCooldown_txt_0->SetText(ftxt);
}


void UCharacterHUD::CommandCheck(int32 idx, int32 activecnt, bool ison)
{
	int32 iarrsize = USkillCommand[idx].SkillCommand_0.Num() < activecnt ? USkillCommand[idx].SkillCommand_0.Num() : activecnt;
	for (int32 i = 0; i < iarrsize; ++i)
	{
		USkillCommand[idx].SkillCommand_0[i]->SetColorAndOpacity(FLinearColor(ison ? FColor(255, 255, 255, 255) : FColor(255, 255, 255, 50)));
	}
}

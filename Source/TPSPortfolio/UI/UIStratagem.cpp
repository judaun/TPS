// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UIStratagem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Stratagem.h"

#define LOCTEXT_NAMESPACE "TPSNameSpace"

void UUIStratagem::InitializeComponent()
{
	UISkillIcon = Cast<UImage>(GetWidgetFromName(TEXT("Skill_Icon")));
	UISkillIcon->ForceLayoutPrepass();
	UTSkillName_txt = Cast<UTextBlock>(GetWidgetFromName(TEXT("Skill_Name")));
	UTSkillName_txt->ForceLayoutPrepass();
	UTSkillStatus_txt = Cast<UTextBlock>(GetWidgetFromName(TEXT("Skill_Status")));
	UTSkillDistance_txt = Cast<UTextBlock>(GetWidgetFromName(TEXT("Skill_Distance")));
}

void UUIStratagem::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUIStratagem::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if(fdistancetime > 0.f)
		fdistancetime-=InDeltaTime;
	else
		SetDistance();
}

void UUIStratagem::SetTime(int32 idx, int32 status, float time)
{
	FText ftxt;

	switch ((ESkillEffectType)status)
	{
		case ESkillEffectType::SKILL_COUNTDOWN:
		ftxt = FText::Format(LOCTEXT("UI_Statagem_CountDown", "CountDown : {0}"), round(time));
		break;

		case ESkillEffectType::SKILL_EFFECT :
			ftxt = FText::Format(LOCTEXT("UI_Statagem_Effect", "Effect : {0}"), round(time));
		break;

		case ESkillEffectType::SKILL_END :
			ftxt = FText::Format(LOCTEXT("UI_Statagem_CoolDown", "Cooldown : {0}"), round(time));
		break;
		default:
		break;
	}
	
	UTSkillStatus_txt->SetText(ftxt);
}

void UUIStratagem::SetSkillData(AStratagem* owner)
{
	if(nullptr == owner) return;

	InitializeComponent();

	owner->func_Stratagem_Time.AddUObject(this, &UUIStratagem::SetTime);
	auto skilldata = owner->GetSkillData();

	FText txt = FText::FromString(*skilldata.Name);
	UTSkillName_txt->SetText(txt);
	
	FString textureaddress = FString::Printf(TEXT("Script/Engine.Texture2D'/Game/UI/Textures/%s.%s'"), *skilldata.Name, *skilldata.Name);
	UTexture2D* pTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *textureaddress));

	UISkillIcon->SetBrushFromTexture(pTexture);
}

void UUIStratagem::SetSkillStatus(FString skillstatus)
{
	UTSkillStatus_txt->SetText(FText::FromString(*skillstatus));
}

void UUIStratagem::SetDistance()
{
	fdistancetime = 1.f;

	FVector vDst = GetWorld()->GetFirstPlayerController()->GetFocalLocation();
	int32 iDist = (vPos - vDst).Length()/100.f;
	FText text = FText::Format(LOCTEXT("UI_Skill_DIstance", "{0}m"), iDist);
	UTSkillDistance_txt->SetText(text);
}

void UUIStratagem::UIActive(bool ison)
{
	
}

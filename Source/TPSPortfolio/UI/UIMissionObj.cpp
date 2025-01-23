// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UIMissionObj.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "MissionObj.h"

#define LOCTEXT_NAMESPACE "TPSNameSpace"

void UUIMissionObj::NativeConstruct()
{
	UTProgress_txt = Cast<UTextBlock>(GetWidgetFromName(FName(TEXT("Txt_Progress"))));
	UPProgress = Cast<UProgressBar>(GetWidgetFromName(FName(TEXT("ProgressBar"))));
}

void UUIMissionObj::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}

void UUIMissionObj::BindActorData(AMissionObj* pobj)
{
	if(!IsValid(pobj)) return;

	//TODO: BindDelegate
	pobj->fDele_Percent.BindUObject(this, &UUIMissionObj::SetProgress_Percent);
	pobj->fDele_Txt.BindUObject(this, &UUIMissionObj::SetProgress_Text);
}

void UUIMissionObj::SetProgress_Percent(float percent)
{
	UPProgress->SetPercent(percent);
}

void UUIMissionObj::SetProgress_Text(FString str)
{
	FText txt = FText::FromString(str);
	UTProgress_txt->SetText(txt);
}

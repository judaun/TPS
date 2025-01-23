// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIMissionObj.generated.h"

/**
 * 
 */
class UTextBlock;
class UProgressBar;
class AMissionObj;

UCLASS()
class TPSPORTFOLIO_API UUIMissionObj : public UUserWidget
{
	GENERATED_BODY()
	
private:
protected:
public:
	void NativeConstruct();
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	void BindActorData(AMissionObj* pobj);

	void SetProgress_Percent(float percent);
	void SetProgress_Text(FString str);
private:
	UPROPERTY()
	TObjectPtr<UTextBlock> UTProgress_txt;

	UPROPERTY()
	TObjectPtr<UProgressBar> UPProgress;
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingMod.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

ALoadingMod::ALoadingMod()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> FCls_Loading(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_Loading.UI_Loading_C'"));
	if (FCls_Loading.Succeeded())
	{
		LoadingWidgetClass = FCls_Loading.Class;
	}
}

void ALoadingMod::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(LoadingWidgetClass))
	{
		LoadingWidget = Cast<UUserWidget>( CreateWidget(GetWorld(), LoadingWidgetClass));

		if (IsValid(LoadingWidget))
		{
			LoadingWidget->AddToViewport();
		}
	}

}

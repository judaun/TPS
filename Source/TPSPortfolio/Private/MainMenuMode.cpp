// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuMode.h"
#include "UObject/ConstructorHelpers.h"
#include "MainMenu.h"

AMainMenuMode::AMainMenuMode()
{
	static ConstructorHelpers::FClassFinder<UMainMenu> FCls_MainMenu(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_Main.UI_Main_C'"));
	if (FCls_MainMenu.Succeeded())
	{
		MainMenuWidgetClass = FCls_MainMenu.Class;
	}
}

void AMainMenuMode::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(MainMenuWidgetClass))
	{
		MainMenuWidget = Cast<UMainMenu>(CreateWidget(GetWorld(), MainMenuWidgetClass));

		if (IsValid(MainMenuWidget))
		{
			MainMenuWidget->AddToViewport();
		}
	}


}

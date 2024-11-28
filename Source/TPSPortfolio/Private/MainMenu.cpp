// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "TPSGameInstance.h"

void UMainMenu::Click_Main()
{
	UE_LOG(LogTemp, Log, TEXT("ClickMain"));
	
	UTPSGameInstance* pGameInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	pGameInstance->MapChange(TEXT("TPSMap1"));
}

void UMainMenu::Click_Test()
{
	UTPSGameInstance* pGameInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	pGameInstance->MapChange(TEXT("TrainningRoom"));

}

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	Btt_Main = Cast<UButton>(GetWidgetFromName(TEXT("Button_Main")));
	Btt_Test = Cast<UButton>(GetWidgetFromName(TEXT("Button_Test")));

	FScriptDelegate deleMain;
	deleMain.BindUFunction(this, FName("Click_Main"));
	Btt_Main->OnClicked.Add(deleMain);


	FScriptDelegate deleTest;
	deleTest.BindUFunction(this, FName(TEXT("Click_Test")));
	Btt_Test->OnClicked.Add(deleTest);

	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerController.h"

#include "Blueprint/UserWidget.h"

ATPSPlayerController::ATPSPlayerController()
{
	// 블루프린트 클래스를 받아온다
	static ConstructorHelpers::FClassFinder<UUserWidget> MainHUDWidgetAsset(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_CrossHair.UI_CrossHair_C'"));

	// TSubclassOf 템플릿 클래스 객체에 블루프린트 클래스를 넣어준다
	if (MainHUDWidgetAsset.Succeeded())
		MainHUDWidgetClass = MainHUDWidgetAsset.Class;
}

ATPSPlayerController::~ATPSPlayerController()
{

}

void ATPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	
	if (IsValid(MainHUDWidgetClass))
	{
		CrossHairHUDWidget = CreateWidget(GetWorld(), MainHUDWidgetClass);

		if (IsValid(CrossHairHUDWidget))
		{
			// 위젯을 뷰포트에 띄우는 함수
			CrossHairHUDWidget->AddToViewport();
		}
	}
	
}

void ATPSPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	
}

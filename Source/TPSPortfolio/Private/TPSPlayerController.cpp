// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerController.h"

#include "Blueprint/UserWidget.h"

ATPSPlayerController::ATPSPlayerController()
{
	// �������Ʈ Ŭ������ �޾ƿ´�
	static ConstructorHelpers::FClassFinder<UUserWidget> MainHUDWidgetAsset(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_CrossHair.UI_CrossHair_C'"));

	// TSubclassOf ���ø� Ŭ���� ��ü�� �������Ʈ Ŭ������ �־��ش�
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
			// ������ ����Ʈ�� ���� �Լ�
			CrossHairHUDWidget->AddToViewport();
		}
	}
	
}

void ATPSPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	
}

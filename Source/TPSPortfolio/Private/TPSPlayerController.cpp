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

	static ConstructorHelpers::FClassFinder<UUserWidget> FCL_HUDCharacter(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/UI_Character.UI_Character_C'"));

	if (FCL_HUDCharacter.Succeeded())
		CharacterHUDWidgetClass = FCL_HUDCharacter.Class;
}

ATPSPlayerController::~ATPSPlayerController()
{

}

void ATPSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	FVector2D ScreenSize;

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ScreenSize);
	}
	
	if (IsValid(MainHUDWidgetClass))
	{
		CrossHairHUDWidget = CreateWidget(GetWorld(), MainHUDWidgetClass);

		if (IsValid(CrossHairHUDWidget))
		{
			// ������ ����Ʈ�� ���� �Լ�
			CrossHairHUDWidget->AddToViewport();
		}
	}
	
	if (IsValid(CharacterHUDWidgetClass))
	{
		CharacterHUDWidget = CreateWidget(GetWorld(), CharacterHUDWidgetClass);
		if (IsValid(CrossHairHUDWidget))
		{
			CharacterHUDWidget->SetDesiredSizeInViewport(FVector2D(528.f, 141.f));
			CharacterHUDWidget->SetPositionInViewport(FVector2D(0.f, ScreenSize.Y- 141.f));
			CharacterHUDWidget->AddToViewport();
		}
	}
}

void ATPSPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	
}

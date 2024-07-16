// Fill out your copyright notice in the Description page of Project Settings.


#include "MinimapComponent.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "PaperSpriteComponent.h"
#include "Engine/CanvasRenderTarget2D.h"

// Sets default values for this component's properties
UMinimapComponent::UMinimapComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	

	MinimapCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	MinimapCameraBoom->SetWorldRotation(FRotator::MakeFromEuler(FVector(0.f, -90.f, 0.f)));

	MinimapCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MinimapCapture"));
	MinimapCapture->ProjectionType = ECameraProjectionMode::Orthographic;

	MinimapSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("MinimapSprite"));
	MinimapSprite->SetWorldRotation(FRotator::MakeFromEuler(FVector(90.f,0.f,-90.f)));
	MinimapSprite->bVisibleInSceneCaptureOnly = true;

	///Script/Engine.CanvasRenderTarget2D'/Game/UI/MinimapRenderTarget2D.MinimapRenderTarget2D'
	ConstructorHelpers::FObjectFinder<UCanvasRenderTarget2D> FOBJ_RenderTarget2D(TEXT("Script/Engine.CanvasRenderTarget2D'/Game/UI/MinimapRenderTarget2D.MinimapRenderTarget2D'"));
	if (FOBJ_RenderTarget2D.Succeeded())
	{
		pRenderTarget = FOBJ_RenderTarget2D.Object;
	}
}


// Called when the game starts
void UMinimapComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	if (MinimapCapture)
	{
		MinimapCapture->SetupAttachment(MinimapCameraBoom);
		MinimapCapture->TextureTarget = pRenderTarget;
	}

}


// Called every frame
void UMinimapComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if(!IsValid(GetOwner()) || nullptr == GetOwner()) return;

	MinimapCameraBoom->SetWorldLocation(GetOwner()->GetActorLocation());
	MinimapSprite->SetWorldLocation(GetOwner()->GetActorLocation());
}


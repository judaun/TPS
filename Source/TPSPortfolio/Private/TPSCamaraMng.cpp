// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSCamaraMng.h"
#include "GameFramework/PlayerController.h"

// Sets default values
ATPSCamaraMng::ATPSCamaraMng()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	InitializeCameraShake();
}

void ATPSCamaraMng::InitializeCameraShake()
{
	AddCameraShakeBase(shake_key::Boom_Small, TEXT("Blueprint'/Game/ThirdPerson/Blueprints/BP_BoomShake'"));
}

void ATPSCamaraMng::AddCameraShakeBase(FString keyname, FString csaddress)
{
	ConstructorHelpers::FClassFinder<UCameraShakeBase> FCL_CameraShake(*csaddress);
	if (FCL_CameraShake.Succeeded())
	{
		TSubclassOf<UCameraShakeBase> pCSB = FCL_CameraShake.Class;
		mapCameraShakeBase.Add(MakeTuple(keyname, pCSB));
	}
}

// Called when the game starts or when spawned
void ATPSCamaraMng::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATPSCamaraMng::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATPSCamaraMng::ClientCameraShake(FString keyname, APlayerController* controller, float scale)
{
	if(!IsValid(controller)) return;

	controller->ClientStartCameraShake(GetCameraShakeBase(*keyname), scale);
}

TSubclassOf<UCameraShakeBase> ATPSCamaraMng::GetCameraShakeBase(FString keyname)
{
	if(mapCameraShakeBase.Num() <= 0) return nullptr;

	auto finder = mapCameraShakeBase.Find(*keyname);

	return *finder;
}


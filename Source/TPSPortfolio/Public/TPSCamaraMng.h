// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSCamaraMng.generated.h"

namespace shake_key
{
	TCHAR* const Boom_Small = TEXT("Boom_Small");
}

#define MAXDISTSHAKE 3000

UCLASS()
class TPSPORTFOLIO_API ATPSCamaraMng : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATPSCamaraMng();

private:
	void InitializeCameraShake();
	void AddCameraShakeBase(FString keyname, FString csaddress);
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
	void ClientCameraShake(FString keyname, APlayerController* controller, float scale);
	TSubclassOf<UCameraShakeBase> GetCameraShakeBase(FString keyname);

	UPROPERTY()
	TMap<FString, TSubclassOf<UCameraShakeBase>> mapCameraShakeBase;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSSoundManager.generated.h"

enum class ESoundAttenuationType : uint8;

namespace sound_key
{
	TCHAR* const HitMark = TEXT("Hitmark");
	TCHAR* const KillMark = TEXT("KillMark");
	TCHAR* const Boom1 = TEXT("Boom1");
	TCHAR* const MetalWalk = TEXT("MetalWalk");
	TCHAR* const Servo1 = TEXT("Servo1");
	TCHAR* const Servo2 = TEXT("Servo2");
	TCHAR* const RobotOff = TEXT("RobotOff");
	TCHAR* const MachineJump = TEXT("MachineJump");
	TCHAR* const Hurt1 = TEXT("Hurt1");
}

UCLASS()
class TPSPORTFOLIO_API ATPSSoundManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATPSSoundManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void StartSoundLocation(FString soundname, const UObject* world, FVector location, ESoundAttenuationType atttype, float volume = 1.f, bool isloop = false);
	void StartSoundLocationRandomPitch(FString soundname, const UObject* world, FVector location, ESoundAttenuationType atttype, float volume = 1.f, bool isloop = false);
	void StartSound(FString soundname, float volume = 1.f, bool isloop = false);
private:
	void AddSound(FString keyname, FString soundaddress);
	void AddAttenuation(ESoundAttenuationType atttype, FString attaddress);
	void InitSound();

private:
	UPROPERTY()
	TMap<FString, USoundBase*> mapSound;

	UPROPERTY()
	TMap<ESoundAttenuationType, USoundAttenuation*> mapSoundAttenuation;
};

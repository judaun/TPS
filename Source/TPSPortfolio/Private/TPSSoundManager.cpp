// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSSoundManager.h"
#include "TPSEnum.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATPSSoundManager::ATPSSoundManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	InitSound();
}

// Called when the game starts or when spawned
void ATPSSoundManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATPSSoundManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATPSSoundManager::StartSoundLocation(FString soundname, const UObject* world, FVector location, ESoundAttenuationType atttype, float volume, bool isloop /*= false*/)
{
	auto pFindSound = mapSound.Find(*soundname);
	if (nullptr != pFindSound)
	{
		auto pFindAtt = mapSoundAttenuation.Find(atttype);
		UGameplayStatics::PlaySoundAtLocation(world, *pFindSound, location, volume,1.f,0.f, pFindAtt == nullptr ? nullptr : *pFindAtt);
	}
}

void ATPSSoundManager::StartSound(FString soundname, float volume, bool isloop /*= false*/)
{
	auto pFindSound = mapSound.Find(*soundname);
	if (nullptr != pFindSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), *pFindSound, volume);
	}
	
}

void ATPSSoundManager::AddSound(FString keyname, FString soundaddress)
{
	ConstructorHelpers::FObjectFinder<USoundBase> FObj(*soundaddress);
	if (FObj.Succeeded())
	{
		USoundBase* pSound = FObj.Object;
		mapSound.Add(MakeTuple(keyname, pSound));
	}
}

void ATPSSoundManager::AddAttenuation(ESoundAttenuationType atttype, FString attaddress)
{
	ConstructorHelpers::FObjectFinder<USoundAttenuation> FObj(*attaddress);
	if (FObj.Succeeded())
	{
		USoundAttenuation* pAttenuation = FObj.Object;
		mapSoundAttenuation.Add(MakeTuple(atttype, pAttenuation));
	}
}

void ATPSSoundManager::InitSound()
{
	AddAttenuation(ESoundAttenuationType::SOUND_LOUD, TEXT("/Script/Engine.SoundAttenuation'/Game/GunSoundAttenuation.GunSoundAttenuation'"));
	AddAttenuation(ESoundAttenuationType::SOUND_SILENCE, TEXT("/Script/Engine.SoundAttenuation'/Game/SilenceAttenuation.SilenceAttenuation'"));
	AddAttenuation(ESoundAttenuationType::SOUND_2D, TEXT("/Script/Engine.SoundAttenuation'/Game/2DAttenuation.2DAttenuation'"));
	///Script/Engine.SoundAttenuation'/Game/2DAttenuation.2DAttenuation'
	AddSound(TEXT("9mmShot"), TEXT("/Script/Engine.SoundWave'/Game/Sounds/9mmPistolShot.9mmPistolShot'"));
	AddSound(TEXT("ReloadHandGun"), TEXT("/Script/Engine.SoundWave'/Game/Sounds/Reload.Reload'"));
	AddSound(TEXT("ShellSound1"), TEXT("Script/Engine.SoundWave'/Game/Sounds/ShellSound1.ShellSound1'"));
	AddSound(TEXT("ShellSound2"), TEXT("Script/Engine.SoundWave'/Game/Sounds/ShellSound2.ShellSound2'"));
	AddSound(TEXT("ShellSound3"), TEXT("Script/Engine.SoundWave'/Game/Sounds/ShellSound3.ShellSound3'"));
	AddSound(TEXT("MagazineDrop"), TEXT("Script/Engine.SoundWave'/Game/Sounds/MagazineDrop.MagazineDrop'"));
	///Script/Engine.SoundWave'/Game/Sounds/MagazineDrop.MagazineDrop'
}


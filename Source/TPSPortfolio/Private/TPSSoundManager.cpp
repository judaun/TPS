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

void ATPSSoundManager::StartSoundLocationRandomPitch(FString soundname, const UObject* world, FVector location, ESoundAttenuationType atttype, float volume, bool isloop /*= false*/)
{
	float fRandPitch = (float)(rand() % 3 - 1) / 20.f + 1.f;
	UE_LOG(LogTemp, Log, TEXT("%f"), fRandPitch);
	auto pFindSound = mapSound.Find(*soundname);
	if (nullptr != pFindSound)
	{
		auto pFindAtt = mapSoundAttenuation.Find(atttype);
		UGameplayStatics::PlaySoundAtLocation(world, *pFindSound, location, volume, fRandPitch, 0.f, pFindAtt == nullptr ? nullptr : *pFindAtt);
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
	AddSound(TEXT("Pistol_Shot"), TEXT("/Script/Engine.SoundWave'/Game/Sounds/Pistol_Shot.Pistol_Shot'"));
	AddSound(TEXT("Rifle_Shot"), TEXT("/Script/Engine.SoundWave'/Game/Sounds/Rifle_Shot.Rifle_Shot'"));
	AddSound(TEXT("Shotgun_Shot"), TEXT("/Script/Engine.SoundWave'/Game/Sounds/Shotgun_Shot.Shotgun_Shot'"));
	AddSound(TEXT("Swosh"), TEXT("/Script/Engine.SoundWave'/Game/Sounds/Whosh.Whosh'"));

	AddSound(TEXT("ReloadHandGun"), TEXT("/Script/Engine.SoundWave'/Game/Sounds/Reload.Reload'"));
	AddSound(TEXT("ShellSound1"), TEXT("Script/Engine.SoundWave'/Game/Sounds/ShellSound1.ShellSound1'"));
	AddSound(TEXT("ShellSound2"), TEXT("Script/Engine.SoundWave'/Game/Sounds/ShellSound2.ShellSound2'"));
	AddSound(TEXT("ShellSound3"), TEXT("Script/Engine.SoundWave'/Game/Sounds/ShellSound3.ShellSound3'"));
	AddSound(TEXT("ShotShellSound"), TEXT("Script/Engine.SoundWave'/Game/Sounds/ShotShellSound.ShotShellSound'"));
	AddSound(TEXT("MagazineDrop"), TEXT("Script/Engine.SoundWave'/Game/Sounds/MagazineDrop.MagazineDrop'"));
	///Script/Engine.SoundWave'/Game/Sounds/MagazineDrop.MagazineDrop'
	AddSound(TEXT("FootStep_Ground_1"), TEXT("/Script/Engine.SoundWave'/Game/Sounds/ground_1.ground_1'"));
	AddSound(TEXT("FootStep_Ground_2"), TEXT("/Script/Engine.SoundWave'/Game/Sounds/ground_2.ground_2'"));
	AddSound(TEXT("FootStep_Ground_3"), TEXT("/Script/Engine.SoundWave'/Game/Sounds/ground_3.ground_3'"));
	AddSound(TEXT("FootStep_Ground_4"), TEXT("/Script/Engine.SoundWave'/Game/Sounds/ground_4.ground_4'"));
	AddSound(TEXT("FootStep_Ground_5"), TEXT("/Script/Engine.SoundWave'/Game/Sounds/ground_5.ground_5'"));
	AddSound(TEXT("ClothFlap"), TEXT("/Script/Engine.SoundWave'/Game/Sounds/ClothFlap.ClothFlap'"));
	AddSound(TEXT("Backpack1"), TEXT("/Script/Engine.SoundWave'/Game/Sounds/Backpack1.Backpack1'"));
	AddSound(TEXT("Backpack2"), TEXT("/Script/Engine.SoundWave'/Game/Sounds/Backpack2.Backpack2'"));
	AddSound(TEXT("Backpack3"), TEXT("/Script/Engine.SoundWave'/Game/Sounds/Backpack2.Backpack2'"));
}


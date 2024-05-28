// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSEffectMng.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"

// Sets default values
ATPSEffectMng::ATPSEffectMng()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	InitEffectManager();
}

// Called when the game starts or when spawned
void ATPSEffectMng::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATPSEffectMng::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATPSEffectMng::AddEffect(FString keyname, FString effectaddress)
{
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> FObj(*effectaddress);
	if (FObj.Succeeded())
	{
		UNiagaraSystem* pEffect = FObj.Object;
		mapEffect.Add(MakeTuple(keyname, pEffect));
	}
}

void ATPSEffectMng::AddDecal(FString keyname, FString decaladdress)
{
	ConstructorHelpers::FObjectFinder<UMaterial> FObj(*decaladdress);
	if (FObj.Succeeded())
	{
		UMaterial* pDecal = FObj.Object;
		mapDecalMaterial.Add(MakeTuple(keyname, pDecal));
	}
}

void ATPSEffectMng::InitEffectManager()
{
	AddEffect(TEXT("Pistol_Fire"), TEXT("/Script/Niagara.NiagaraSystem'/Game/Effects/FX_PistolFire.FX_PistolFire'"));
	AddEffect(TEXT("Rifle_Fire"), TEXT("/Script/Niagara.NiagaraSystem'/Game/Effects/FX_RifleFire.FX_RifleFire'"));
	AddEffect(TEXT("Shotgun_Fire"), TEXT("/Script/Niagara.NiagaraSystem'/Game/Effects/FX_ShotgunFire.FX_ShotgunFire'"));
	AddEffect(TEXT("BulletHit"), TEXT("Script/Niagara.NiagaraSystem'/Game/Effects/FX_GunHit.FX_GunHit'"));
	AddEffect(TEXT("Dust"), TEXT("/Script/Niagara.NiagaraSystem'/Game/Effects/FX_Dust.FX_Dust'"));
	AddEffect(Eff_key::DistortionBoom,TEXT("/Script/Niagara.NiagaraSystem'/Game/Effects/FX_Boom.FX_Boom'"));
	AddEffect(Eff_key::HitEffect, TEXT("/Script/Niagara.NiagaraSystem'/Game/Effects/FX_Splat.FX_Splat'"));
	AddEffect(Eff_key::BloodEffect, TEXT("/Script/Niagara.NiagaraSystem'/Game/Effects/FX_Blood.FX_Blood'"));
	AddEffect(Eff_key::CannonEffect, TEXT("/Script/Niagara.NiagaraSystem'/Game/Effects/FX_Cannon.FX_Cannon'"));
	//Decal
	AddDecal(TEXT("BulletHole"), TEXT("/Script/Engine.Material'/Game/Effects/M_BulletHole.M_BulletHole'"));
	AddDecal(TEXT("FootStep_L"), TEXT("/Script/Engine.Material'/Game/Effects/M_FootStepL.M_FootStepL'"));
	AddDecal(TEXT("FootStep_R"), TEXT("/Script/Engine.Material'/Game/Effects/M_FootStepR.M_FootStepR'"));
	///Script/Engine.Material'/Game/Effects/M_FootStepL.M_FootStepL'
}

void ATPSEffectMng::SpawnEffect(FString keyname, const UObject* WorldContextObject, FVector Location, FRotator Rotation /*= FRotator::ZeroRotator*/, FVector Scale /*= FVector(1.f)*/, bool bAutoDestroy /*= true*/)
{
	auto mapfind = mapEffect.Find(*keyname);
	if (nullptr == mapfind) return;

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContextObject, *mapfind, Location, Rotation, Scale, bAutoDestroy);
}

void ATPSEffectMng::SpawnDecal(FString keyname, TObjectPtr<UWorld> WorldContextObject, float lifetime, FVector Location, FRotator Rotation /*= FRotator::ZeroRotator*/, FVector Scale /*= FVector(1.f)*/, float fadedistancesize /*= 0.01f*/)
{
	if (nullptr == WorldContextObject)return;

	auto mapfind = mapDecalMaterial.Find(keyname);
	if (nullptr == mapfind) return;

	ADecalActor* pDecalActor = WorldContextObject->SpawnActor<ADecalActor>(Location, Rotation);
	pDecalActor->SetDecalMaterial(*mapfind);
	pDecalActor->SetLifeSpan(lifetime);
	pDecalActor->GetDecal()->DecalSize = Scale;
	pDecalActor->GetDecal()->FadeScreenSize = fadedistancesize;
	pDecalActor->GetDecal()->FadeDuration = 1.f;
}

UNiagaraSystem* ATPSEffectMng::GetEffect(FString keyname)
{
	auto mapfind = mapEffect.Find(*keyname);
	if (nullptr == mapfind) return nullptr;

	return *mapfind;
}


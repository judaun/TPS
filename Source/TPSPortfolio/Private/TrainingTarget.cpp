// Fill out your copyright notice in the Description page of Project Settings.


#include "TrainingTarget.h"
#include "Components/CapsuleComponent.h"
#include "TPSGameInstance.h"
#include "TPSEffectMng.h"
#include "TPSSoundManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

// Sets default values
ATrainingTarget::ATrainingTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bIsDissolve = false;
	fDissolve = 0.f;

	pMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
}

void ATrainingTarget::HitAction()
{
	bIsDissolve = true;
	SetCollisionAll(false);

	if (!GetWorldTimerManager().IsTimerActive(dissolvetimehandle))
		GetWorldTimerManager().SetTimer(dissolvetimehandle, [this]() {Recovery();}, 5.f, false);
}

void ATrainingTarget::Recovery()
{
	bIsDissolve = false;
	SetCollisionAll(true);
}

void ATrainingTarget::Dissolving(float culvevalue)
{
	if (!IsValid(pM_Dynamic)) return;
	
	if (bIsDissolve)
	{
		if (fDissolve < 1.f)
			fDissolve += culvevalue;
	}
	else
	{
		if (fDissolve > 0.f)
			fDissolve -= culvevalue;
	}
	
	pM_Dynamic->SetScalarParameterValue(FName(TEXT("Dissolve")), fDissolve);
}

void ATrainingTarget::SetCollisionAll(bool isOn)
{
	pMesh->SetCollisionEnabled(isOn ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ATrainingTarget::BeginPlay()
{
	Super::BeginPlay();
	
	pMesh->AddLocalRotation(FRotator(0.f,0.f,90.f));
	pM_Dynamic = pMesh->CreateDynamicMaterialInstance(0);

}

float ATrainingTarget::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UTPSGameInstance* pGameInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	pGameInstance->StartSoundLocationRandomPitch(sound_key::SteelHit, GetWorld(), GetActorLocation(), ESoundAttenuationType::SOUND_LOUD, 1.f);
	HitAction();
	return Damage;
}

// Called every frame
void ATrainingTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Dissolving(DeltaTime);
}


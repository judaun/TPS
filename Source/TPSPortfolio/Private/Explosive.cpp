// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosive.h"
#include "TPSGameInstance.h"
#include "TPSEffectMng.h"
#include "TPSCamaraMng.h"
#include "TPSSoundManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TPSPortfolioCharacter.h"

// Sets default values
AExplosive::AExplosive()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bIsExploding = false;
}

void AExplosive::Explode()
{
	if(!IsValid(this)) return;
	if(bIsExploding) return;
	bIsExploding = true;

	FVector vSrc = GetActorLocation();
	FVector vDst = GetWorld()->GetFirstPlayerController()->GetFocalLocation();
	float fDist = (vSrc - vDst).Length();
	float fShakeScale = 1.f - (fDist / MAXDISTSHAKE);

	UTPSGameInstance* pInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (pInstance)
	{
		pInstance->SpawnEffect(Eff_key::BoomSmall, GetWorld(), GetActorLocation(), FRotator::ZeroRotator, FVector(2.f), true);
		if (fShakeScale > 0.f)
			pInstance->ClientCameraShake(shake_key::Boom_Small, GetWorld()->GetFirstPlayerController(), fShakeScale);

		pInstance->StartSoundLocation(sound_key::Boom1, GetWorld(), GetActorLocation(), ESoundAttenuationType::SOUND_LOUD);
	}

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	UGameplayStatics::ApplyRadialDamage(GetWorld(), iExpodeDmg, GetActorLocation(), fExplodeRadius, nullptr, ActorsToIgnore, this, GetWorld()->GetFirstPlayerController(), false, ECollisionChannel::ECC_WorldDynamic);

	Destroy();
}

// Called when the game starts or when spawned
void AExplosive::BeginPlay()
{
	Super::BeginPlay();
	
}

void AExplosive::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	if(!bIsCollisionExplode) return;
	if (!IsValid(this)) return;
	if (Other->IsA(ATPSPortfolioCharacter::StaticClass())) return;

	Explode();
}

void AExplosive::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if(!bIsCollisionExplode) return;
	if (!IsValid(this)) return;
	if(OtherActor->IsA(ATPSPortfolioCharacter::StaticClass())) return;

	Explode();
}

// Called every frame
void AExplosive::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AExplosive::DeferredInitialize(bool isProjectile)
{
	pMesh = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), TEXT("Mesh"));
	pMesh->RegisterComponent();
	RootComponent = pMesh;
	pMesh->SetNotifyRigidBodyCollision(true);
	pMesh->SetGenerateOverlapEvents(true);
	pMesh->SetSimulatePhysics(true);
	pMesh->SetCollisionProfileName(TEXT("MeshEffect"));
	if (isProjectile)
	{
		
	}
	else
	{
		UStaticMesh* newMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("/Script/Engine.StaticMesh'/Game/Props/Lyra/Grenade/Mesh/Grenade.Grenade'")));
		if (newMesh)
			pMesh->SetStaticMesh(newMesh);
	}
}

void AExplosive::SetExplosiveValue(bool isCollision, float radius, int32 dmg, float explodetime)
{
	bIsCollisionExplode = isCollision;
	fExplodeRadius = radius;
	iExpodeDmg = dmg;
	fExplodeTime = explodetime;
	if (fExplodeTime > 0.f)
	{
		GetWorldTimerManager().SetTimer(Explodetimehandle, [this](){Explode();}, fExplodeTime, false);
	}

}

void AExplosive::AddImpuse(FVector impulsevector)
{
	if(!IsValid(pMesh)) return;

	pMesh->AddImpulse(impulsevector, NAME_None, true);
}


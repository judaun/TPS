// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "TPSPortfolioCharacter.h"
#include "Enemy.h"
#include "TPSGameInstance.h"
#include "TPSSoundManager.h"
#include "TPSCamaraMng.h"
#include "TPSEffectMng.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/DamageEvents.h"
#include "Components/CapsuleComponent.h"
#include "Components/DynamicMeshComponent.h"
#include "TPSGameInstanceSubsystem.h"
#include "UDynamicMesh.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	pMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = pMesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FObj_STM (TEXT("/Script/Engine.StaticMesh'/Game/Props/Meshes/ETC/Gun_BulletFX.Gun_BulletFX'"));
	if (FObj_STM.Succeeded())
	{
		pMesh->SetStaticMesh(FObj_STM.Object);
		pMesh->SetCollisionProfileName(FName(TEXT("Projectile")));
		pMesh->SetNotifyRigidBodyCollision(true);
		pMesh->SetGenerateOverlapEvents(true);
	}
	
}

void AProjectile::InitializeMesh()
{
	


}

void AProjectile::DestroyProjectile(FVector hitpos)
{
	if (!IsValid(this)) return;
	UTPSGameInstance* pInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (fExplodeRadius > 0.f)
	{
		FVector vSrc = hitpos;
		FVector vDst = GetWorld()->GetFirstPlayerController()->GetFocalLocation();
		float fDist = (vSrc - vDst).Length();
		float fShakeScale = 1.f - (fDist / MAXDISTSHAKE);

		//TODO : 피해범위 별 이펙트 변경
		pInstance->SpawnEffect(fExplodeRadius > 500.f ? Eff_key::BoomBig : Eff_key::BoomSmall, GetWorld(), vSrc, FRotator::ZeroRotator, FVector(2.f), true);
		if (fExplodeRadius > 500.f)
		{
			pInstance->SpawnEffect(Eff_key::VolumeFog, GetWorld(), vSrc, FRotator::ZeroRotator, FVector(1.f), true);
			auto subsys = pInstance->GetSubsystem<UTPSGameInstanceSubsystem>();
			subsys->SpawnRVTEffect(vSrc, FVector(fExplodeRadius),TEXT(""));
			pInstance->StartSoundLocation(sound_key::Boom1, GetWorld(), GetActorLocation(), ESoundAttenuationType::SOUND_LOUD);
		}
		else
			pInstance->StartSoundLocationRandomPitch(sound_key::Gatling,GetWorld(), vSrc, ESoundAttenuationType::SOUND_LOUD);

		if (fShakeScale > 0.f)
			pInstance->ClientCameraShake(fExplodeRadius > 500.f ?  shake_key::Boom_Big : shake_key::Boom_Small, GetWorld()->GetFirstPlayerController(), fShakeScale);
		
		TArray<TEnumAsByte<EObjectTypeQuery>> arr_Objtype;
		TArray<AActor*> IgnoreActors;
		TArray<AActor*> OutActors;
		if(UKismetSystemLibrary::SphereOverlapActors(GetWorld(), vSrc, fExplodeRadius, arr_Objtype, nullptr, IgnoreActors, OutActors))
		{
			FRadialDamageEvent DmgEvent;
			DmgEvent.DamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
			DmgEvent.Origin = GetActorLocation();
			DmgEvent.Params = FRadialDamageParams(iDmg, fExplodeRadius);

			for (auto elem_vec : OutActors)
			{
				TArray<FHitResult> HitList;
				FVector elemLoc = elem_vec->GetActorLocation();
				FVector FakeHitNorm = (DmgEvent.Origin - elemLoc).GetSafeNormal();

				if (elem_vec->IsA(AEnemy::StaticClass()) || elem_vec->IsA(ATPSPortfolioCharacter::StaticClass()))
				{
					auto component = elem_vec->GetComponentByClass<UCapsuleComponent>();

					FHitResult Hit(elem_vec, component, elemLoc, FakeHitNorm);
					HitList.Add(Hit);
					DmgEvent.ComponentHits = HitList;
					elem_vec->TakeDamage(iDmg, DmgEvent, GetWorld()->GetFirstPlayerController(), this);
				}
				else if (elem_vec->ActorHasTag(FName(TEXT("MassActor"))))
				{
					elem_vec->Destroy();
				}
			}
		}

		

	}
	else
	{
		pInstance->StartSoundLocation(sound_key::ImpactGround, GetWorld(), GetActorLocation(), ESoundAttenuationType::SOUND_LOUD, 0.5f);
	}
	Destroy();
	
	
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	vStart = GetActorLocation();
}

void AProjectile::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp,Other,OtherComp,bSelfMoved,HitLocation,HitNormal,NormalImpulse,Hit);


	if (OtherComp->IsA(UDynamicMeshComponent::StaticClass()) && fExplodeRadius > 0.f)
	{
		//UE_LOG(LogTemp,Log,TEXT("DynamicMeshComponent Hit"));
		auto subsys = GetGameInstance()->GetSubsystem<UTPSGameInstanceSubsystem>();
		auto DynamicMeshCom = Cast<UDynamicMeshComponent>(OtherComp);

		subsys->MeshBoolean(DynamicMeshCom, Hit.Location, FVector(fExplodeRadius));

	}
	
	DestroyProjectile(HitLocation);
}

void AProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if(IsValid(OtherActor) && nullptr != pOwner)
	UGameplayStatics::ApplyDamage(OtherActor, iDmg, pOwner->GetInstigatorController(), pOwner.Get(), nullptr);

	DestroyProjectile(GetActorLocation());
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector vSrc = GetActorLocation();
	FVector vDst = vSrc + vDirection*fSpeed;

	FHitResult Result;
	if (SetActorLocation(vDst, true, &Result))
	{
		if (Result.GetActor())
		{
			DestroyProjectile(Result.ImpactPoint);
			//UE_LOG(LogTemp, Log, TEXT("%s"), *Result.GetActor()->GetName());
		}
	}
	
	
	if((GetActorLocation() - vStart).Length() > 12000.f && IsValid(this))
		Destroy();
}

void AProjectile::SetData(bool isgravity, float expolderadius, int32 dmg, AActor* owner)
{
	//if(!isgravity)
	iDmg = dmg;
	fExplodeRadius = expolderadius;
	pOwner = TWeakObjectPtr<AActor>(owner);
}

void AProjectile::SetDirection(FVector direction, float speed)
{
	SetActorRotation(direction.Rotation());
	vDirection = direction;
	fSpeed = speed;
}


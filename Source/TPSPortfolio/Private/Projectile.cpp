// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "TPSPortfolioCharacter.h"
#include "Enemy.h"

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

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	vStart = GetActorLocation();
}

void AProjectile::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp,Other,OtherComp,bSelfMoved,HitLocation,HitNormal,NormalImpulse,Hit);
	if (IsValid(this))
		Destroy();
}

void AProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if(IsValid(OtherActor) && OtherActor->IsA(ATPSPortfolioCharacter::StaticClass()) && nullptr != pOwner)
	UGameplayStatics::ApplyDamage(OtherActor, iDmg, pOwner->GetInstigatorController(), pOwner.Get(), nullptr);

	if (IsValid(this))
		Destroy();
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
			if(IsValid(this))
				Destroy();
			//UE_LOG(LogTemp, Log, TEXT("%s"), *Result.GetActor()->GetName());
		}
	}
	
	
	if((GetActorLocation() - vStart).Length() > 3000.f && IsValid(this))
		Destroy();
}

void AProjectile::SetData(bool isgravity, bool expolsive, int32 dmg, AActor* owner)
{
	//if(!isgravity)
	iDmg = dmg;

	pOwner = TWeakObjectPtr<AActor>(owner);
}

void AProjectile::SetDirection(FVector direction, float speed)
{
	SetActorRotation(direction.Rotation());
	vDirection = direction;
	fSpeed = speed;
}


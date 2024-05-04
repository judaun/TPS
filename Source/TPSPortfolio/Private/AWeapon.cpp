// Fill out your copyright notice in the Description page of Project Settings.


#include "AWeapon.h"
#include "Bullet.h"
#include "Magazine.h"
#include "TPSPortfolioCharacter.h"
#include "TPSEnum.h"

// Sets default values
AWeapon::AWeapon() : Equipment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}


AWeapon::AWeapon(FEquipmentTable* equipdata) : Equipment(equipdata)
{
	PrimaryActorTick.bCanEverTick = true;
	if (nullptr == equipdata)
		return;

	FString strTypeName = GetWeaponTypeName(equipdata->WeaponType);
	FString meshaddress = FString::Printf(TEXT("/Script/Engine.SkeletalMesh'/Game/Props/Meshes/%s/%s.%s'"), *strTypeName, *equipdata->Name, *equipdata->Name);
	InitializeMesh(meshaddress);

	collisionParams.AddIgnoredActor(this);
	collisionParams.AddIgnoredActor(GetOwner());


	iCurrentCapacity = equipdata->iBaseCapacity;
	iCurrentMagazine = equipdata->iBaseMagazine;
}


void AWeapon::InitializeMesh(FString weaponaddress)
{
	pMesh = NewObject<USkeletalMeshComponent>(this, USkeletalMeshComponent::StaticClass(), TEXT("Weapon"));
	/*pMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));*/
	pMesh->RegisterComponent();
	RootComponent = pMesh;

	USkeletalMesh* newMesh = Cast< USkeletalMesh >(StaticLoadObject(USkeletalMesh::StaticClass(), NULL, *weaponaddress));
	//static ConstructorHelpers::FObjectFinder<USkeletalMesh> FObj_WeaponSkmesh(*weaponaddress);
	if (newMesh)
		pMesh->SetSkeletalMesh(newMesh);

	pMesh->SetCollisionProfileName(TEXT("NoCollision"));

}

FString AWeapon::GetWeaponTypeName(EWeaponType weapontype)
{
	switch (weapontype)
	{
	case EWeaponType::WEAPON_HANDGUN:
		return TEXT("Pistol");
	case EWeaponType::WEAPON_RIFLE:
		return TEXT("Rifle");
	case EWeaponType::WEAPON_SHOTGUN:
		return TEXT("Shotgun");
	case EWeaponType::WEAPON_MELEE:
		return TEXT("Melee");
	case EWeaponType::WEAPON_THROW:
		return TEXT("Throw");
	default:
		return TEXT("");
	}
}

void AWeapon::InitMagazineMesh(FString magazineaddress)
{
	if (!IsValid(pMesh)) return;

	FRotator rotMagazine = pMesh->GetSocketRotation(TEXT("magazineSocket"));
	FVector vMagazine = pMesh->GetSocketLocation(TEXT("magazineSocket"));


	FTransform SpawnTransform(rotMagazine, vMagazine);
	pMagazine = GetWorld()->SpawnActorDeferred<AMagazine>(AMagazine::StaticClass(), SpawnTransform);
	if (pMagazine)
	{
		pMagazine->DeferredInitialize(magazineaddress);
		pMagazine->FinishSpawning(SpawnTransform);
	}

	FName MagazineSocket(TEXT("magazineSocket"));
	if (pMesh->DoesSocketExist(MagazineSocket) && nullptr != pMagazine)
	{
		pMagazine->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale, MagazineSocket);
	}
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	InitMagazineMesh(TEXT("/Script/Engine.StaticMesh'/Game/Props/Meshes/ETC/M9-Magazine.M9-Magazine'"));
}

void AWeapon::SetPlayer(ATPSPortfolioCharacter* character)
{
	pCharacter = TWeakObjectPtr<ATPSPortfolioCharacter>(character);
}

void AWeapon::DeferredInitialize(FEquipmentTable* equipdata)
{
	if (nullptr == equipdata)
		return;

	SetData(equipdata);
	FString strTypeName = GetWeaponTypeName(equipdata->WeaponType);
	FString meshaddress = FString::Printf(TEXT("/Script/Engine.SkeletalMesh'/Game/Props/Meshes/%s/%s.%s'"), *strTypeName, *equipdata->Name, *equipdata->Name);
	InitializeMesh(meshaddress);

	collisionParams.AddIgnoredActor(this);
	collisionParams.AddIgnoredActor(GetOwner());

	iCurrentCapacity = equipdata->iBaseCapacity;
	iCurrentMagazine = equipdata->iBaseMagazine;
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::AttackTrace()
{
	if (iCurrentCapacity <= 0) return;
	--iCurrentCapacity;

	//Bullet spawn pos
	FVector vfireStart = pMesh->GetSocketLocation(TEXT("BulletStart"));
	FVector vDir;
	if (pCharacter.IsValid())
	{
		pCharacter->SetAttacking(true);
		vDir = (pCharacter->GetAimPosVector() - vfireStart).GetSafeNormal();
	}
		
	
	FVector vfireEnd = vfireStart + (vDir * FEquipData.fBaseRange);
	
	const UWorld* currentWorld = GetWorld();
	FHitResult hitResult;

	UE_LOG(LogTemp, Log, TEXT("AttTrace"));
	DrawDebugLine(currentWorld, vfireStart, vfireEnd, FColor::Red, false, 0.3f);
	if (currentWorld->LineTraceSingleByChannel(hitResult, vfireStart, vfireEnd, ECC_Visibility, collisionParams))
	{
		if (hitResult.GetActor())
		{
			auto hitActor = hitResult.GetActor();
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Hit Actor Name: %s"), *hitActor->GetName()));
		}
	}

	FRotator rotBullet = pMesh->GetSocketRotation(TEXT("BulletOut"));
	FVector vBulletOut = pMesh->GetSocketLocation(TEXT("BulletOut"));
	GetWorld()->SpawnActor<ABullet>(vBulletOut, rotBullet);
}

void AWeapon::Reload()
{
	--iCurrentMagazine;

	iCurrentCapacity = FEquipData.iBaseCapacity;
	InitMagazineMesh(TEXT("/Script/Engine.StaticMesh'/Game/Props/Meshes/ETC/M9-Magazine.M9-Magazine'"));

}

void AWeapon::ReloadStart()
{
	if (IsValid(pMagazine))
	{
		pMagazine->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		pMagazine->MagazineOut();
		pMagazine = nullptr;
	}
}

void AWeapon::AttackStart()
{
	auto powner = GetOwner();

	
	if (!pCharacter.IsValid()) return;

	if (!pCharacter->GetisAiming())
	{
		//melee
		return;
	}

	switch (FEquipData.ProjectileType)
	{
	case EProjectileType::PROJECTILE_TRACE:
		if(!GetWorldTimerManager().IsTimerActive(Firetimehandle))
		GetWorldTimerManager().SetTimer(Firetimehandle, [this](){AttackTrace(); }, FEquipData.fBaseAttInterval/1000.f, true, 0.f);
		break;
	case EProjectileType::PROJECTILE_PROJECTILE:
		break;
	}
}

void AWeapon::AttackStop()
{
	if (Firetimehandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(Firetimehandle);
	}
}

float AWeapon::GetBulletrate()
{
	float fRemainrate = (float)iCurrentCapacity / (FEquipData.iBaseCapacity == 0 ? 10 : FEquipData.iBaseCapacity);
	
	return  clamp(fRemainrate, 0.f, 1.f);
}

bool AWeapon::IsFullCapacity()
{
	return iCurrentCapacity >= FEquipData.iBaseCapacity;
}

bool AWeapon::IsPosibleReload()
{
	return iCurrentMagazine > 0;
}


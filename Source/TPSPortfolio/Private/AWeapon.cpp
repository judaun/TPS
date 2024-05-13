// Fill out your copyright notice in the Description page of Project Settings.


#include "AWeapon.h"
#include "Bullet.h"
#include "Magazine.h"
#include "TPSPortfolioCharacter.h"
#include "Curves/CurveVector.h"
#include "TPSEnum.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "TPSGameInstance.h"

#define MAXIMUM_RECOIL_POS 100

// Sets default values
AWeapon::AWeapon() : Equipment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	UE_LOG(LogTemp, Log, TEXT("UCameraShakeBase load"));
	
	ConstructorHelpers::FClassFinder<UCameraShakeBase> FCL_CameraShake(TEXT("Blueprint'/Game/ThirdPerson/Blueprints/BP_TPSCameraShake'"));
	if (FCL_CameraShake.Succeeded())
	{
		UE_LOG(LogTemp, Log, TEXT("UCameraShakeBase load Success"));
		CS_Attack = FCL_CameraShake.Class;
	}

	ConstructorHelpers::FObjectFinder<UCurveVector> FOBJ_Curve(TEXT("/Script/Engine.CurveVector'/Game/ThirdPerson/Curves/RecoilCurve.RecoilCurve'"));
	if (FOBJ_Curve.Succeeded())
	{
		UE_LOG(LogTemp, Log, TEXT("CameraRecoilCurve load Success"));
		CameraRecoilCurve = FOBJ_Curve.Object;
	}

	fPitchRecoil = 0.f;
	fYawRecoil = 0.f;
	fAimRate = 0.f;
	fFireMenual = 0.f;
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


AWeapon::~AWeapon()
{
	
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

	FString strFireEffName = FString::Printf(TEXT("%s_Fire"), *FEquipData.Name);
	UTPSGameInstance* pGameInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (pGameInstance)
	{
		pNiagaraCom = UNiagaraFunctionLibrary::SpawnSystemAttached(pGameInstance->GetEffect(strFireEffName), pMesh, TEXT("Muzzle"), FVector(0.f), FRotator(0.f), EAttachLocation::Type::KeepRelativeOffset, false);
		if (pNiagaraCom)
		{
			pNiagaraCom->Deactivate();
		}
	}

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

void AWeapon::InitMagazineMesh()
{
	if (!IsValid(pMesh)) return;

	FString magazineaddress = FString::Printf(TEXT("/Script/Engine.StaticMesh'/Game/Props/Meshes/ETC/%s_Magazine.%s_Magazine'"), *FEquipData.Name, *FEquipData.Name);

	FRotator rotMagazine = pMesh->GetSocketRotation(TEXT("MagazineSocket"));
	FVector vMagazine = pMesh->GetSocketLocation(TEXT("MagazineSocket"));


	FTransform SpawnTransform(rotMagazine, vMagazine);
	pMagazine = GetWorld()->SpawnActorDeferred<AMagazine>(AMagazine::StaticClass(), SpawnTransform);
	if (pMagazine)
	{
		pMagazine->DeferredInitialize(magazineaddress);
		pMagazine->FinishSpawning(SpawnTransform);
	}

	FName MagazineSocket(TEXT("MagazineSocket"));
	if (pMesh->DoesSocketExist(MagazineSocket) && nullptr != pMagazine)
	{
		pMagazine->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale, MagazineSocket);
	}
}

void AWeapon::InitTimeLine()
{
	if (nullptr == CameraRecoilCurve) return;

	FOnTimelineVector CameraRecoilCallback;

	FOnTimelineEventStatic TimelineFinishCallback;

	CameraRecoilCallback.BindUFunction(this, FName("OnCameraRecoilProgress"));
	TimelineFinishCallback.BindUFunction(this, FName("OnRecoilTimelineFinish"));

	RecoilTimeline.AddInterpVector(CameraRecoilCurve, CameraRecoilCallback);
	RecoilTimeline.SetTimelineFinishedFunc(TimelineFinishCallback);

	RecoilTimeline.SetTimelineLength(0.3f);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	InitMagazineMesh();
	InitTimeLine();

}

void AWeapon::SetPlayer(ATPSPortfolioCharacter* character)
{
	if (nullptr == character) return;
	pCharacter = TWeakObjectPtr<ATPSPortfolioCharacter>(character);
	pCharacter->func_Player_Aimrate.AddUObject(this, &AWeapon::SetAimRate);
}

void AWeapon::DeferredInitialize(FEquipmentTable* equipdata)
{
	if (nullptr == equipdata)
		return;
	///Script/Engine.Skeleton'/Game/Props/Lyra/%s/Mesh/%s.%s'
	SetData(equipdata);
	FString strTypeName = GetWeaponTypeName(equipdata->WeaponType);
	FString meshaddress = FString::Printf(TEXT("Script/Engine.Skeleton'/Game/Props/Lyra/%s/Mesh/%s.%s'"), *strTypeName, *equipdata->Name, *equipdata->Name);
	InitializeMesh(meshaddress);

	//USkeletalMesh* newMesh = Cast< USkeletalMesh >(StaticLoadObject(USkeletalMesh::StaticClass(), NULL, *weaponaddress));
	FString strAnimAddress = FString::Printf(TEXT("/Script/Engine.AnimSequence'/Game/Props/Lyra/%s/Animations/%s_Fire.%s_Fire'"), *strTypeName, *equipdata->Name, *equipdata->Name);
	pShotAnim = Cast<UAnimSequence>(StaticLoadObject(UAnimSequence::StaticClass(), NULL, *strAnimAddress));
	

	collisionParams.AddIgnoredActor(this);
	collisionParams.AddIgnoredActor(GetOwner());

	iCurrentCapacity = equipdata->iBaseCapacity;
	iCurrentMagazine = equipdata->iBaseMagazine;
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RecoilTimeline.TickTimeline(DeltaTime);
	if (fPitchRecoil < 0.f)
	{
		fPitchRecoil += DeltaTime*3.f;
		GetWorld()->GetFirstPlayerController()->AddPitchInput(DeltaTime * 3.f);
	}

	if (fYawRecoil > 0.1f)
	{
		fYawRecoil -= DeltaTime * 3.f;
		GetWorld()->GetFirstPlayerController()->AddYawInput(DeltaTime * 3.f);
	}
	else if (fYawRecoil < -0.1f)
	{
		fYawRecoil += DeltaTime * 3.f;
		GetWorld()->GetFirstPlayerController()->AddYawInput(DeltaTime * 3.f);
	}

	if (fFireMenual > 0.f)
		fFireMenual -= DeltaTime;
}

void AWeapon::AttackTrace()
{
	if (iCurrentCapacity <= 0) return;
	--iCurrentCapacity;

	fFireMenual += FEquipData.fBaseAttInterval / 1000.f;

	//Bullet spawn pos
	FVector vfireStart = pMesh->GetSocketLocation(TEXT("Muzzle"));
	FVector vDir;
	int32 iFirecnt = FEquipData.WeaponType == EWeaponType::WEAPON_SHOTGUN ? 10 : 1;
	for (int32 i = 0; i < iFirecnt; ++i)
	{
		if (pCharacter.IsValid())
		{
			pCharacter->PlayAttack();
			FVector vAimPos = pCharacter->GetAimPosVector() + GetAimrateRecoilPosition();
			vDir = (vAimPos - vfireStart).GetSafeNormal();
		}


		FVector vfireEnd = vfireStart + (vDir * FEquipData.fBaseRange);

		const UWorld* currentWorld = GetWorld();
		FHitResult hitResult;

		//UE_LOG(LogTemp, Log, TEXT("AttTrace"));
		//DrawDebugLine(currentWorld, vfireStart, vfireEnd, FColor::Red, false, 0.3f);
		if (currentWorld->LineTraceSingleByChannel(hitResult, vfireStart, vfireEnd, ECC_Visibility, collisionParams))
		{
			if (hitResult.GetActor())
			{
				auto hitActor = hitResult.GetActor();
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Hit Actor Name: %s"), *hitActor->GetName()));
			}
			SetSpawnDecal(hitResult.Location, hitResult.ImpactNormal.Rotation());
		}

		
	}

	FRotator rotBullet = pMesh->GetSocketRotation(TEXT("ShellEject"));
	FVector vBulletOut = pMesh->GetSocketLocation(TEXT("ShellEject"));
	auto pBullet = GetWorld()->SpawnActor<ABullet>(vBulletOut, rotBullet);
	if (FEquipData.WeaponType == EWeaponType::WEAPON_SHOTGUN && IsValid(pBullet))
		pBullet->ShotShell(true);

	if (pNiagaraCom)
	{
		pNiagaraCom->SetWorldRotation(vDir.Rotation());
		pNiagaraCom->Activate(true);
	}

	float fShakeScale = FEquipData.WeaponType == EWeaponType::WEAPON_SHOTGUN ? 1.f : FEquipData.WeaponType == EWeaponType::WEAPON_RIFLE ? 0.15f:0.3f;

	GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(CS_Attack, fShakeScale);
	
	RecoilTimeline.PlayFromStart();
	
	if (pShotAnim)
	{
		pMesh->PlayAnimation(pShotAnim, false);
	}
		
	UTPSGameInstance* pGameInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	FString strSoundName = FString::Printf(TEXT("%s_Shot"), *FEquipData.Name);
	if (nullptr != pGameInstance) pGameInstance->StartSoundLocationRandomPitch(*strSoundName, GetWorld(), GetActorLocation(), ESoundAttenuationType::SOUND_LOUD);
}

void AWeapon::Reload()
{
	--iCurrentMagazine;

	iCurrentCapacity = FEquipData.iBaseCapacity;
	
	InitMagazineMesh();

}

void AWeapon::ReloadStart()
{
	if (IsValid(pMagazine))
	{
		pMagazine->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		pMagazine->MagazineOut();
		pMagazine = nullptr;

		iCurrentCapacity = 0;

		UTPSGameInstance* pGameInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (nullptr != pGameInstance) pGameInstance->StartSoundLocation(TEXT("ReloadHandGun"), GetWorld(), GetActorLocation(), ESoundAttenuationType::SOUND_2D);
	}
}

void AWeapon::AttackStart()
{
	auto powner = GetOwner();

	
	if (!pCharacter.IsValid()) return;

	if (!pCharacter->GetisAiming())
	{
		pCharacter->PlayAttack(true);
		return;
	}

	switch (FEquipData.ProjectileType)
	{
	case EProjectileType::PROJECTILE_TRACE:
		if(fFireMenual <= 0.f)AttackTrace();
		//if (!GetWorldTimerManager().IsTimerActive(Firetimehandle) && fFireMenual<=0.f)
		//GetWorldTimerManager().SetTimer(Firetimehandle, [this](){AttackTrace(); }, FEquipData.fBaseAttInterval/1000.f, true, 0.f);
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

void AWeapon::SetHide(bool hide)
{
	SetActorHiddenInGame(hide);
	pMagazine->SetActorHiddenInGame(hide);
}

FVector AWeapon::GetAimrateRecoilPosition()
{
	float fX = 0.f;
	float fY = 0.f;
	float fZ = 0.f;

	float fCharacterAimrate = fAimRate * 0.5f; // 0 ~ 0.5
	float fWeaponAimrate = 1.f - (FEquipData.fBaseAccuracy/1000.f); // 0 ~ 1
	float fMaxlen = MAXIMUM_RECOIL_POS * (fCharacterAimrate + fWeaponAimrate) +1; // 1000 * (0~1.5)+zerodivideº¸Á¤

	fX = rand() % (int)fMaxlen;
	fY = rand() % (int)fMaxlen;
	fZ = rand() % (int)fMaxlen;

	fX *= rand() % 2 == 0 ? -1.f : 1.f;
	fY *= rand() % 2 == 0 ? -1.f : 1.f;
	fZ *= rand() % 2 == 0 ? -1.f : 1.f;

	UE_LOG(LogTemp, Log, TEXT("MaxLen :%f, X:%f, Y:%f, Z:%f"), fMaxlen, fX, fY, fZ);

	return FVector(fX, fY, fZ);
}

bool AWeapon::IsFullCapacity()
{
	return iCurrentCapacity >= FEquipData.iBaseCapacity;
}

bool AWeapon::IsPosibleReload()
{
	return iCurrentMagazine > 0;
}

void AWeapon::OnCameraRecoilProgress(FVector CameraRecoil)
{
	float fRecoil_Y = CameraRecoil.Y;
	fRecoil_Y *= rand() % 2 == 0 ? -1.f : 1.f;

	GetWorld()->GetFirstPlayerController()->AddPitchInput(CameraRecoil.Z);
	GetWorld()->GetFirstPlayerController()->AddYawInput(fRecoil_Y);

	fPitchRecoil += CameraRecoil.Z;
	fYawRecoil += fRecoil_Y;
}

void AWeapon::OnRecoilTimelineFinish()
{
	//UE_LOG(LogTemp, Log, TEXT("TimeLineEnd"));
}

void AWeapon::SetSpawnDecal(FVector Location, FRotator Rotator)
{
	Rotator.Pitch += 90;

	UTPSGameInstance* pGameInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (pGameInstance)
	{
		pGameInstance->SpawnEffect(TEXT("BulletHit"), GetWorld(), Location, Rotator, FVector(0.5f), true);
		pGameInstance->SpawnDecal(TEXT("BulletHole"), GetWorld(), 5.f, Location, Rotator, FVector(4.f), 0.002f);
	}
}


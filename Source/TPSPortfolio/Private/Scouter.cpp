// Fill out your copyright notice in the Description page of Project Settings.


#include "Scouter.h"
#include "QuadFootIK.h"
#include "TPSTag.h"
#include "TPSSoundManager.h"
#include "Kismet/GameplayStatics.h"
#include "TPSGameInstance.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "TPSEffectMng.h"
#include "Projectile.h"
#include "EnemyController.h"
#include "TPSCamaraMng.h"

AScouter::AScouter(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{

}

void AScouter::ServoMotorSound()
{
	if(!IsValid(this)) return;

	UTPSGameInstance* pInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (pInstance)
	{
		bool bIsEven = rand() % 2 == 0;
		pInstance->StartSoundLocation(bIsEven ? sound_key::Servo1 : sound_key::Servo2, GetWorld(), GetActorLocation(), ESoundAttenuationType::SOUND_SILENCE, 0.5f);
	}
}

void AScouter::BeginPlay()
{
	Super::BeginPlay();

	if (!GetWorldTimerManager().IsTimerActive(Soundtimehandle))
		GetWorldTimerManager().SetTimer(Soundtimehandle, [this]() {ServoMotorSound(); }, 3.f, true);

	Cast<AEnemyController>(GetController())->ChangeLongRange(true);

	GetCapsuleComponent()->SetMassOverrideInKg(NAME_None, 5000.f);
}

void AScouter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AScouter::InitializeDefaultComponent()
{
	Super::InitializeDefaultComponent();

	GetCapsuleComponent()->SetCapsuleSize(60.f, 60.0f);
	GetCapsuleComponent()->SetHiddenInGame(false);
	pFootIK = NewObject<UQuadFootIK>(this, UQuadFootIK::StaticClass(), TEXT("FootIK"));
	pFootIK->RegisterComponent();
	auto pQuadFoot= Cast<UQuadFootIK>(pFootIK);
	pQuadFoot->SetLegData(true, true, FName("Bone016"), FName("SocketLF"));//LF
	pQuadFoot->SetLegData(false, true, FName("Bone012"), FName("SocketRF"));//RF
	pQuadFoot->SetLegData(true, false, FName("Bone004"), FName("SocketLB"));//LB
	pQuadFoot->SetLegData(false, false, FName("Bone008"), FName("SocketRB"));//RB
}

void AScouter::InitializeMeshComponent()
{
	FString strMeshName = FString::Printf(TEXT("/Script/Engine.SkeletalMesh'/Game/Enemy/%s/Meshes/SK_%s.SK_%s'"), *EnemyData.Name, *EnemyData.Name, *EnemyData.Name);
	USkeletalMesh* pMesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), NULL, *strMeshName));
	FString strAnimName = FString::Printf(TEXT("Blueprint'/Game/Enemy/%s/ABP_%s.ABP_%s_C'"), *EnemyData.Name, *EnemyData.Name, *EnemyData.Name);
	UClass* pAnimclass = StaticLoadClass(UAnimInstance::StaticClass(), NULL, *strAnimName);
	//UAnimBlueprint* pBPAnim = LoadAnim
	if (pMesh)
	{
		GetMesh()->SetSkeletalMesh(pMesh);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -80.f), FRotator(0.f, 0.f, 0.f));
		GetMesh()->SetCollisionProfileName(FName(TEXT("Enemy")));
		if (pAnimclass)GetMesh()->SetAnimInstanceClass(pAnimclass);

	}

	UTPSGameInstance* pInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (pInstance)
	{
		pNiagaraCom = UNiagaraFunctionLibrary::SpawnSystemAttached(pInstance->GetEffect(Eff_key::CannonEffect), GetMesh(), TEXT("Muzzle"), FVector(0.f), FRotator(0.f), EAttachLocation::Type::KeepRelativeOffset, false);
		if (pNiagaraCom)
		{
			pNiagaraCom->Deactivate();
		}
	}

	pProjectileClass = StaticLoadClass(AProjectile::StaticClass(), NULL, TEXT("Blueprint'/Game/ThirdPerson/Blueprints/BP_Projectile.BP_Projectile_C'"));
}

void AScouter::SetEnemyData(FEnemyTable* enemydata)
{
	Super::SetEnemyData(enemydata);
	InitializeDefaultComponent();
	InitializeMeshComponent();
}

void AScouter::BreakBone()
{
	Super::BreakBone();
	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));

	GetMesh()->BreakConstraint(FVector::ZeroVector, FVector::ZeroVector, FName("connectBone002"));
	GetMesh()->BreakConstraint(FVector::ZeroVector, FVector::ZeroVector, FName("connectBone003"));
	GetMesh()->BreakConstraint(FVector::ZeroVector, FVector::ZeroVector, FName("connectBone004"));
	GetMesh()->BreakConstraint(FVector::ZeroVector, FVector::ZeroVector, FName("connectBone005"));
	GetMesh()->BreakConstraint(FVector::ZeroVector, FVector::ZeroVector, FName("headBone"));
	GetMesh()->BreakConstraint(FVector::ZeroVector, FVector::ZeroVector, FName("gun001"));

	GetMesh()->SetSimulatePhysics(true);

	UTPSGameInstance* pInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (pInstance)
	{
		pInstance->StartSoundLocation(sound_key::RobotOff, GetWorld(), GetActorLocation(), ESoundAttenuationType::SOUND_LOUD, 0.5f);
	}

	if (Soundtimehandle.IsValid())
		GetWorldTimerManager().ClearTimer(Soundtimehandle);
}

void AScouter::ActiveEffect(FVector direction)
{
	if(!IsValid(pNiagaraCom)) return;
	//발포 이펙트 방향 지정 및 활성화
	pNiagaraCom->SetWorldRotation(direction.Rotation());
	pNiagaraCom->Activate(true);
	//Projectile 엑터 생성
	FActorSpawnParameters actorparam;
	actorparam.Owner = this;
	auto pActor = GetWorld()->SpawnActor<AProjectile>(pProjectileClass,GetMesh()->GetSocketLocation(FName(TEXT("Muzzle"))),direction.Rotation(), actorparam);
	pActor->SetDirection(direction,15.f);
	pActor->SetData(true,true, GetEnemyData().ProjectileDamage, this);

	FVector vSrc = GetActorLocation();
	FVector vDst = GetWorld()->GetFirstPlayerController()->GetFocalLocation();

	float fDist = (vSrc - vDst).Length();
	float fShakeScale = 1.f - (fDist / MAXDISTSHAKE);

	UTPSGameInstance* pInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (pInstance)
	{
		if (fShakeScale > 0.f)
			pInstance->ClientCameraShake(shake_key::Boom_Small, GetWorld()->GetFirstPlayerController(), fShakeScale);
	}
}

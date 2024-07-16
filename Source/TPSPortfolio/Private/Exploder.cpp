// Fill out your copyright notice in the Description page of Project Settings.


#include "Exploder.h"
#include "QuadFootIK.h"
#include "TPSTag.h"
#include "TPSSoundManager.h"
#include "Kismet/GameplayStatics.h"
#include "TPSGameInstance.h"
#include "Components/CapsuleComponent.h"

AExploder::AExploder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsAttacking = false;
}


void AExploder::ServoMotorSound()
{
	if (!IsValid(this)) return;

	UTPSGameInstance* pInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (pInstance)
	{
		bool bIsEven = rand() % 2 == 0;
		pInstance->StartSoundLocation(bIsEven ? sound_key::Servo1 : sound_key::Servo2, GetWorld(), GetActorLocation(), ESoundAttenuationType::SOUND_SILENCE, 0.5f);
	}
}

void AExploder::BeginPlay()
{
	Super::BeginPlay();

	if(!GetWorldTimerManager().IsTimerActive(Soundtimehandle))
		GetWorldTimerManager().SetTimer(Soundtimehandle, [this](){ServoMotorSound();},3.f, true);

	GetCapsuleComponent()->SetMassOverrideInKg(NAME_None, 1000.f);
}

void AExploder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AExploder::InitializeDefaultComponent()
{
	Super::InitializeDefaultComponent();
	
	GetCapsuleComponent()->SetCapsuleSize(40.f, 40.0f);

	//GetCapsuleComponent()->SetHiddenInGame(false);
	pFootIK = NewObject<UQuadFootIK>(this, UQuadFootIK::StaticClass(), TEXT("FootIK"));
	pFootIK->RegisterComponent();

	DamageCapsuleComponent =NewObject<UCapsuleComponent>(this, UCapsuleComponent::StaticClass(), TEXT("DamageCapsule"));
	DamageCapsuleComponent->RegisterComponent();
	DamageCapsuleComponent->SetGenerateOverlapEvents(true);
	DamageCapsuleComponent->SetCapsuleSize(100.f,50.f);
	DamageCapsuleComponent->ComponentTags.Add(FName(cap_tag::EnemyDmgCap));
	DamageCapsuleComponent->SetHiddenInGame(false);
	DamageCapsuleComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	DamageCapsuleComponent->SetCollisionProfileName(FName(TEXT("DamageCapsule")));
	
	DamageCapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AExploder::InitializeMeshComponent()
{
	FString strMeshName = FString::Printf(TEXT("/Script/Engine.SkeletalMesh'/Game/Enemy/%s/Meshes/SK_%s.SK_%s'"), *EnemyData.Name, *EnemyData.Name, *EnemyData.Name);
	USkeletalMesh* pMesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), NULL, *strMeshName));
	FString strAnimName = FString::Printf(TEXT("Blueprint'/Game/Enemy/%s/ABP_%s.ABP_%s_C'"), *EnemyData.Name, *EnemyData.Name, *EnemyData.Name);
	UClass* pAnimclass = StaticLoadClass(UAnimInstance::StaticClass(), NULL, *strAnimName);
	//UAnimBlueprint* pBPAnim = LoadAnim
	if (pMesh)
	{
		GetMesh()->SetSkeletalMesh(pMesh);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -45.f), FRotator(0.f, -90.f, 0.f));
		GetMesh()->SetCollisionProfileName(FName(TEXT("Enemy")));
		if(pAnimclass)GetMesh()->SetAnimInstanceClass(pAnimclass);
	}
}

void AExploder::SetEnemyData(FEnemyTable* enemydata)
{
	Super::SetEnemyData(enemydata);
	InitializeDefaultComponent();
	InitializeMeshComponent();
}

void AExploder::BreakBone()
{
	Super::BreakBone();
	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->BreakConstraint(FVector::ZeroVector, FVector::ZeroVector, FName("SpiderBot_Thigh_Front_Right"));
	GetMesh()->BreakConstraint(FVector::ZeroVector, FVector::ZeroVector, FName("SpiderBot_Thigh_Front_Left"));
	GetMesh()->BreakConstraint(FVector::ZeroVector, FVector::ZeroVector, FName("SpiderBot_Thigh_Rear_Left"));
	GetMesh()->BreakConstraint(FVector::ZeroVector, FVector::ZeroVector, FName("SpiderBot_Thigh_Rear_Right"));
	GetMesh()->BreakConstraint(FVector::ZeroVector, FVector::ZeroVector, FName("SpiderBot_Abdomen"));

	GetMesh()->SetSimulatePhysics(true);

	UTPSGameInstance* pInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (pInstance)
	{
		pInstance->StartSoundLocation(sound_key::RobotOff, GetWorld(),GetActorLocation(),ESoundAttenuationType::SOUND_LOUD, 0.5f);
	}

	if(Soundtimehandle.IsValid())
		GetWorldTimerManager().ClearTimer(Soundtimehandle);
}

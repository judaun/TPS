// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldItem.h"

#include "TPSEnum.h"
#include "TPSGameInstance.h"
#include "TPSPortfolioCharacter.h"
#include "Inventory.h"

// Sets default values
AWorldItem::AWorldItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	iFieldItemIdx = -1;
	iItemCnt = 0;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> FObj_Mat (TEXT("/Script/Engine.Material'/Game/Props/Lyra/Generic/Materials/M_OutLine.M_OutLine'"));
	if (FObj_Mat.Succeeded())
	{
		pOutLineMaterial = FObj_Mat.Object;
	}
}

// Called when the game starts or when spawned
void AWorldItem::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(pStOutline))
	{
		pM_Dynamic = pStOutline->CreateDynamicMaterialInstance(0);
	}
	else if (IsValid(pSkOutline))
	{
		pM_Dynamic = pSkOutline->CreateDynamicMaterialInstance(0);
	}
}

// Called every frame
void AWorldItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AddActorLocalRotation(FRotator(0.f,1.f,0.f));
}

void AWorldItem::ObtainItem(ATPSPortfolioCharacter* owner)
{
	if(!IsValid(owner)) return;
	if(!IsValid(this)) return;

	if (iFieldItemIdx < 0)
	{
		//Equipment
		owner->LoadWeapon(fEquipData.Itemkey);
	}
	else
	{
		//Item
		owner->AddItem(iFieldItemIdx, iItemCnt);
	}

	Destroy();
}

void AWorldItem::ObtainRangeIn(bool isoutline)
{
	if(!IsValid(pM_Dynamic)) return;

	pM_Dynamic->SetScalarParameterValue(TEXT("LineSize"), isoutline ? 1.f: 0.f);
}

void AWorldItem::InitFieldItem(int32 index, int32 itemcnt)
{
	//Component Init
	pStMesh = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), TEXT("StaticMesh"));
	pStMesh->RegisterComponent();
	RootComponent = pStMesh;
	pStOutline = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), TEXT("StOutlineMesh"));
	pStOutline->RegisterComponent();
	pStOutline->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	//MeshInit
	FString strAddress;
	switch (index)
	{
		case fielditemtype::GrenadeAmmo: strAddress = TEXT("/Script/Engine.StaticMesh'/Game/Props/Lyra/Grenade/Mesh/Grenade.Grenade'");
			break;
		case fielditemtype::WeaponAmmo: strAddress = TEXT("/Script/Engine.StaticMesh'/Game/Props/Meshes/ETC/Rifle_Magazine.Rifle_Magazine'");
			break;
		case fielditemtype::HealthPack: strAddress = TEXT("/Script/Engine.StaticMesh'/Game/Props/Lyra/Healthpack/Mesh/Healthpack.Healthpack'");
			break;
	}
	UStaticMesh* pMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *strAddress));
	if (pMesh)
	{
		pStMesh->SetStaticMesh(pMesh);
		pStOutline->SetStaticMesh(pMesh);
		if(pOutLineMaterial)
		pStOutline->SetMaterial(0, pOutLineMaterial);
	}
	pStMesh->SetRelativeScale3D(FVector(2.f));
	pStMesh->SetMobility(EComponentMobility::Movable);
	pStMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	pStMesh->SetEnableGravity(false);
	pStMesh->SetSimulatePhysics(false);

	pStOutline->SetMobility(EComponentMobility::Movable);
	pStOutline->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	pStOutline->SetEnableGravity(false);
	pStOutline->SetSimulatePhysics(false);

	//SetData
	iFieldItemIdx = index;
	iItemCnt = itemcnt;
}

void AWorldItem::InitFieldEquip(FEquipmentTable equipdata)
{
	//Component Init
	pSkMesh = NewObject<USkeletalMeshComponent>(this, USkeletalMeshComponent::StaticClass(), TEXT("SkeletalMesh"));
	pSkMesh->RegisterComponent();
	RootComponent = pSkMesh;
	pSkOutline = NewObject<USkeletalMeshComponent>(this, USkeletalMeshComponent::StaticClass(), TEXT("SkOutLineMesh"));
	pSkOutline->RegisterComponent();
	pSkOutline->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	//MeshInit
	FString strTypeName = GetWeaponTypeName(equipdata.WeaponType);
	FString meshaddress = FString::Printf(TEXT("Script/Engine.Skeleton'/Game/Props/Lyra/%s/Mesh/%s.%s'"), *strTypeName, *equipdata.Name, *equipdata.Name);
	USkeletalMesh* pMesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), NULL, *meshaddress));
	if (pMesh)
	{
		pSkMesh->SetSkeletalMesh(pMesh);
		pSkOutline->SetSkeletalMesh(pMesh);
		if(pOutLineMaterial)
			pSkOutline->SetMaterial(0, pOutLineMaterial);
	}
	pSkMesh->SetMobility(EComponentMobility::Movable);
	pSkMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	pSkMesh->SetEnableGravity(false);
	pSkMesh->SetSimulatePhysics(false);

	pSkOutline->SetMobility(EComponentMobility::Movable);
	pSkOutline->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	pSkOutline->SetEnableGravity(false);
	pSkOutline->SetSimulatePhysics(false);

	//SetData
	SetData(equipdata);
}

void AWorldItem::InitFieldEquip(FEquipmentTable* equipdata)
{
	pSkMesh = NewObject<USkeletalMeshComponent>(this, USkeletalMeshComponent::StaticClass(), TEXT("SkeletalMesh"));
	pSkMesh->RegisterComponent();
	RootComponent = pSkMesh;
	pSkOutline = NewObject<USkeletalMeshComponent>(this, USkeletalMeshComponent::StaticClass(), TEXT("SkOutLineMesh"));
	pSkOutline->RegisterComponent();
	pSkOutline->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	//MeshInit
	FString strTypeName = GetWeaponTypeName(equipdata->WeaponType);
	FString meshaddress = FString::Printf(TEXT("Script/Engine.Skeleton'/Game/Props/Lyra/%s/Mesh/%s.%s'"), *strTypeName, *equipdata->Name, *equipdata->Name);
	USkeletalMesh* pMesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), NULL, *meshaddress));
	if (pMesh)
	{
		pSkMesh->SetSkeletalMesh(pMesh);
		pSkOutline->SetSkeletalMesh(pMesh);
		if (pOutLineMaterial)
			pSkOutline->SetMaterial(0, pOutLineMaterial);
		
	}	
	pSkMesh->SetMobility(EComponentMobility::Movable);
	pSkMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	pSkMesh->SetEnableGravity(false);
	pSkMesh->SetSimulatePhysics(false);

	pSkOutline->SetMobility(EComponentMobility::Movable);
	pSkOutline->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	pSkOutline->SetEnableGravity(false);
	pSkOutline->SetSimulatePhysics(false);

	//SetData
	SetData(equipdata);
}

FString AWorldItem::GetWeaponTypeName(EWeaponType weapontype)
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


void AWorldItem::SetData(FEquipmentTable equipdata)
{
	fEquipData.Itemkey = equipdata.Itemkey;
	fEquipData.Name = equipdata.Name;
	fEquipData.EquipType = equipdata.EquipType;
	fEquipData.WeaponType = equipdata.WeaponType;
	fEquipData.ProjectileType = equipdata.ProjectileType;
	fEquipData.iBaseDmg = equipdata.iBaseDmg;
	fEquipData.fBaseAttInterval = equipdata.fBaseAttInterval;
	fEquipData.fBaseAccuracy = equipdata.fBaseAccuracy;
	fEquipData.fBaseRange = equipdata.fBaseRange;
	fEquipData.iBaseCapacity = equipdata.iBaseCapacity;
	fEquipData.iBaseMagazine = equipdata.iBaseMagazine;
	fEquipData.fBaseReloadTime = equipdata.fBaseReloadTime;
}


void AWorldItem::SetData(FEquipmentTable* equipdata)
{
	fEquipData.Itemkey = equipdata->Itemkey;
	fEquipData.Name = equipdata->Name;
	fEquipData.EquipType = equipdata->EquipType;
	fEquipData.WeaponType = equipdata->WeaponType;
	fEquipData.ProjectileType = equipdata->ProjectileType;
	fEquipData.iBaseDmg = equipdata->iBaseDmg;
	fEquipData.fBaseAttInterval = equipdata->fBaseAttInterval;
	fEquipData.fBaseAccuracy = equipdata->fBaseAccuracy;
	fEquipData.fBaseRange = equipdata->fBaseRange;
	fEquipData.iBaseCapacity = equipdata->iBaseCapacity;
	fEquipData.iBaseMagazine = equipdata->iBaseMagazine;
	fEquipData.fBaseReloadTime = equipdata->fBaseReloadTime;
}
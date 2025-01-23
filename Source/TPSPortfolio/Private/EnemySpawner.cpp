// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include "TPSGameInstance.h"
#include "TPSEnemyMng.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "TPSEffectMng.h"
#include "TPSPortfolioCharacter.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	neartime = 0.f;
	spawncnt = 5;
	spawntime = 2.f;
	bisspawn = false;

	DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	SetRootComponent(DefaultRoot);

	//pEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SpawnerEffect"));
	
	UTPSGameInstance* pGameInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (pGameInstance)
	{
		pEffectComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), pGameInstance->GetEffect(Eff_key::SpawnFog), GetActorLocation(),  FRotator(0.f), FVector(10.f),false,false);
		if (pEffectComp)
		{
			pEffectComp->Deactivate();
		}
	}
}

void AEnemySpawner::NearCharacter()
{
	TArray<AActor*> TA_Actor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATPSPortfolioCharacter::StaticClass(), TA_Actor);
	if (TA_Actor.IsEmpty())
	{ 
		bisspawn = false;
		return;
	}

	FVector vSpawnerLoc = GetActorLocation();
	float fDist = 1000.f;
	for (auto elem_TA : TA_Actor)
	{
		if (!IsValid(elem_TA)) continue;
		float fLen = (vSpawnerLoc - elem_TA->GetActorLocation()).Length();
		if (fLen < fDist)
		{
			bisspawn = true;
			return;
		}
	}
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	/*DrawDebugBox(GetWorld(), GetActorLocation(), FVector(1000.f), FColor::Cyan, false, 30.f);*/
	if (pEffectComp) {
		pEffectComp->SetRelativeLocation(GetActorLocation());
		pEffectComp->Activate();
	}
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	spawntime -= DeltaTime;
	if (spawntime <= 0.f)
	{
		Spawn();
	}
	
}

void AEnemySpawner::Spawn()
{
	if(spawncnt <= 0) {
		if (pEffectComp)
		{
			pEffectComp->Deactivate();
		}
		Destroy();
		return;
	}

	UTPSGameInstance* pGameInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (pGameInstance)
	{
		//random 1 ~ 2
		int32 iSpawnKey = rand() % 3 +1;

		/*switch (eType)
		{
		case EEnemyType::ENEMY_ASSAULT: iSpawnKey = EnemyKey::ENEMY_EXPLODER;
			break;
		case EEnemyType::ENEMY_SCOUT: iSpawnKey = EnemyKey::ENEMY_SCOUTER;
			break;
		}*/
		pGameInstance->SpawnEnemy(iSpawnKey, GetWorld(), GetActorLocation(), GetActorRotation(), true);
	}

	--spawncnt;
	spawntime = 5.f;
}


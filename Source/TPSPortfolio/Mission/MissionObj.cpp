// Fill out your copyright notice in the Description page of Project Settings.


#include "Mission/MissionObj.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "UIMissionObj.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnemySpawner.h"

// Sets default values
AMissionObj::AMissionObj()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

#pragma region ComponentInitialize
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	RootComponent = CapsuleComp;
	CapsuleComp->InitCapsuleSize(600.f,400.f);

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	MeshComp->SetupAttachment(CapsuleComp);

	ConstructorHelpers::FObjectFinder<USkeletalMesh> FObj_Mesh (TEXT("/Script/Engine.SkeletalMesh'/Game/VigilanteContent/Vehicles/West_Radar_Patriot/SK_West_Radar_Patriot.SK_West_Radar_Patriot'"));
	if (FObj_Mesh.Succeeded())
	{
		MeshComp->SetSkeletalMesh(FObj_Mesh.Object);
		MeshComp->SetCollisionProfileName(FName(TEXT("BlockAll")));
		MeshComp->SetNotifyRigidBodyCollision(true);
		MeshComp->SetGenerateOverlapEvents(true);
	}

	pUIComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("UI_Progress"));
	pUIComponent->SetupAttachment(CapsuleComp);
	pUIComponent->SetRelativeLocation(FVector(0.f, 0.f, 300.f));
	ConstructorHelpers::FClassFinder<UUserWidget> FCls_UI(TEXT("/Game/UI/UI_MissionProgress.UI_MissionProgress_C"));

	if (FCls_UI.Succeeded())
	{
		pUIComponent->SetWidgetClass(FCls_UI.Class);
		pUIComponent->SetWidgetSpace(EWidgetSpace::Screen);
		pUIComponent->SetDrawSize(FVector2D(400.f, 120.f));
		pUIComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		pUIComponent->SetHiddenInGame(true);
	}

	pInterationUIComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("UI_Interaction"));
	pInterationUIComponent->SetupAttachment(CapsuleComp);
	ConstructorHelpers::FClassFinder<UUserWidget> FCls_Inter_UI(TEXT("/Game/UI/UI_Interation.UI_Interation_C"));
	if (FCls_Inter_UI.Succeeded())
	{
		pInterationUIComponent->SetWidgetClass(FCls_Inter_UI.Class);
		pInterationUIComponent->SetWidgetSpace(EWidgetSpace::Screen);
		pInterationUIComponent->SetDrawSize(FVector2D(200.f, 200.f));
		pInterationUIComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		pInterationUIComponent->SetHiddenInGame(true);
	}

	MinimapSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Minimap_Sprite"));
	MinimapSprite->SetupAttachment(CapsuleComp);
	MinimapSprite->SetWorldLocation(FVector(0.f, 0.f, 300.f));
	MinimapSprite->SetWorldRotation(FRotator::MakeFromEuler(FVector(90.f, 0.f, 0.f)));
	MinimapSprite->SetWorldScale3D(FVector(0.5f));
	MinimapSprite->bVisibleInSceneCaptureOnly = true;

	ConstructorHelpers::FObjectFinder<UPaperSprite> FObj_Sprite (TEXT("/Script/Paper2D.PaperSprite'/Game/UI/Textures/EnemyMark_Sprite.EnemyMark_Sprite'"));
	if (FObj_Sprite.Succeeded())
	{
		MinimapSprite->SetSprite(FObj_Sprite.Object);
	}

	///Script/AIModule.EnvQuery'/Game/ThirdPerson/AI/EQ_Enemyspawner.EQ_Enemyspawner'
	ConstructorHelpers::FObjectFinder<UEnvQuery> FObj_Eqs (TEXT("/Script/AIModule.EnvQuery'/Game/ThirdPerson/AI/EQ_Enemyspawner.EQ_Enemyspawner'"));
	if (FObj_Eqs.Succeeded())
	{
		Eqs = FObj_Eqs.Object;
	}
#pragma endregion

	Tags.Add(FName(TEXT("InteractionActor")));
	fPhaseTime = 10.f;
	fUIUpdateTime = 0.f;
	curState = MISSION_READY;
}

void AMissionObj::ActivateAnimation()
{

}

void AMissionObj::ActivateUI()
{
	pUIComponent->SetHiddenInGame(false);
}

void AMissionObj::DeActivateUI()
{
	pUIComponent->SetHiddenInGame(true);
}

void AMissionObj::SpawnEnemy(FVector location)
{
	FTransform SpawnTransform(FRotator::ZeroRotator, location);
	AEnemySpawner* pSpawner = GetWorld()->SpawnActorDeferred<AEnemySpawner>(AEnemySpawner::StaticClass(), SpawnTransform);
	if (IsValid(pSpawner))
	{
		pSpawner->FinishSpawning(SpawnTransform);
	}

}

void AMissionObj::PhaseProgress()
{
	int8 iState = curState;
	++iState;
	curState = (MissionState)iState;

	FString strTxt;
	switch (curState)
	{
	case MISSION_READY:
		break;
	case MISSION_PHASE1: strTxt = TEXT("Progress : Phase1");
		break;
	case MISSION_PHASE2: strTxt = TEXT("Progress : Phase2");
		ExecuteEqs();
		break;
	case MISSION_PHASE3: strTxt = TEXT("Progress : Phase3");
		ExecuteEqs();
		break;
	case MISSION_END:
		break;
	default:
		break;
	}

	fDele_Txt.ExecuteIfBound(*strTxt);

	UE_LOG(LogTemp, Log, TEXT("%d"), iState);

	if (curState == MISSION_END)
	{
		if (GetWorldTimerManager().IsTimerActive(Activetimehandle))
			GetWorldTimerManager().ClearTimer(Activetimehandle);
		MissionComplete();
	}
}

void AMissionObj::PhaseReset()
{
	curState = MISSION_READY;
}

void AMissionObj::TimerCheck(float DeltaTime)
{
	if(curState == MISSION_READY || curState == MISSION_END) return;

	float fRemainTime = GetWorldTimerManager().GetTimerRemaining(Activetimehandle);
	if(fRemainTime < 0.f) fRemainTime = 0.f;

	float fState  = ((float)curState -1.f) / 3.f;
	float fpropercent = (fPhaseTime - fRemainTime) / fPhaseTime / 3.f + fState;

	if(fUIUpdateTime < 1.f) fUIUpdateTime += DeltaTime;
	else
	{
		fUIUpdateTime = 0.f;
		fDele_Percent.ExecuteIfBound(fpropercent);
	}
}

void AMissionObj::ExecuteEqs()
{
	pEqsRequest.Execute(EEnvQueryRunMode::RandomBest5Pct, this,&AMissionObj::EqsFinished);
}

// Called when the game starts or when spawned
void AMissionObj::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(pUIComponent))
	{
		auto pWiget = Cast<UUIMissionObj>(pUIComponent->GetWidget());
		pWiget->BindActorData(this);
	}

	if (IsValid(Eqs))
	{
		pEqsRequest = FEnvQueryRequest(Eqs,this);
	}
}

// Called every frame
void AMissionObj::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TimerCheck(DeltaTime);
}

void AMissionObj::Interaction()
{
	InteractionUI(false);

	if (!GetWorldTimerManager().IsTimerActive(Activetimehandle))
		GetWorldTimerManager().SetTimer(Activetimehandle, [this]() {PhaseProgress(); }, fPhaseTime,true,0.f);

	ActivateAnimation();
	ActivateUI();
	
}

void AMissionObj::InteractionUI(bool ison)
{
	if(ison && curState != MISSION_READY) return;

	pInterationUIComponent->SetHiddenInGame(!ison);
}

void AMissionObj::MissionComplete()
{
	DeActivateUI();
}

void AMissionObj::EqsFinished(TSharedPtr<FEnvQueryResult> result)
{
	FVector vLoc = result->GetItemAsLocation(0);
	SpawnEnemy(vLoc);
}


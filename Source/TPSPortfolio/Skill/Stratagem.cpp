// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/Stratagem.h"
#include "Projectile.h"
#include "TPSPortfolioCharacter.h"
#include "Components/SplineMeshComponent.h"
#include "Components/SplineComponent.h"
#include "TPSGameInstance.h"
#include "TPSEffectMng.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "UIStratagem.h"
#include "Components/WidgetComponent.h"
#include "TPSSoundManager.h"


// Sets default values
AStratagem::AStratagem() : UPlayerSkill()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	eGemeffecttype = ESkillEffectType::SKILL_READY;
	
	pMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	pMesh->RegisterComponent();
	RootComponent = pMesh;

	pMesh->SetCollisionProfileName(TEXT("MeshEffect"));
	pMesh->SetNotifyRigidBodyCollision(false);
	pMesh->SetGenerateOverlapEvents(false);
	pMesh->SetSimulatePhysics(false);

	ConstructorHelpers::FObjectFinder<UStaticMesh> FObj_Mesh(TEXT("/Script/Engine.StaticMesh'/Game/Props/Lyra/Grenade/Mesh/Grenade.Grenade'"));
	if (FObj_Mesh.Succeeded())
	{
		pMesh->SetStaticMesh(FObj_Mesh.Object);
	}
	
#pragma region StratagemUI
	
	pUIComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("UI"));
	pUIComponent->RegisterComponent();
	pUIComponent->SetupAttachment(RootComponent);
	pUIComponent->SetRelativeLocation(FVector(0.f, 0.f, 300.f));

	ConstructorHelpers::FClassFinder<UUserWidget> FCls_UI(TEXT("/Game/UI/UI_Stratagem.UI_Stratagem_C"));
	
	///WidgetBlueprint'/Game/UI/UI_Stratagem'
	if (FCls_UI.Succeeded())
	{
		UE_LOG(LogTemp, Log, TEXT("pUserWidget load"));
		pUIComponent->SetWidgetClass(FCls_UI.Class);
		pUIComponent->SetWidgetSpace(EWidgetSpace::Screen);
		pUIComponent->SetDrawSize(FVector2D(400.f, 190.f));
		pUIComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		pUIComponent->SetHiddenInGame(true);
	}

#pragma endregion
}

void AStratagem::DeferredInitialize()
{

	
	Spline_Path = NewObject<USplineComponent>(this, USplineComponent::StaticClass(), TEXT("Spline"));
	Spline_Path->RegisterComponent();

	SplineMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Shapes/Shape_Cylinder.Shape_Cylinder'")));
	SplineMaterial = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, TEXT("/Script/Engine.Material'/Game/Effects/M_AimTrail.M_AimTrail'")));

	UTPSGameInstance* pGameInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (pGameInstance)
	{
		pNiagaraCom = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),pGameInstance->GetEffect(Eff_key::Stratagem),GetActorLocation(),FRotator::ZeroRotator,FVector(1.f),false,false);
		pNiagaraCom->Deactivate();
	}


}

void AStratagem::InitDelegate()
{
	auto pUI = Cast<UUIStratagem>(pUIComponent->GetUserWidgetObject());
	if(IsValid(pUI))
		pUI->SetSkillData(this);
}

void AStratagem::Deployment()
{
	ChangeState(ESkillEffectType::SKILL_COUNTDOWN);
	if(pNiagaraCom) 
	{
		pNiagaraCom->SetRelativeLocation(GetActorLocation());
		pNiagaraCom->Activate(true);
	}
	pMesh->SetSimulatePhysics(false);
	pMesh->SetNotifyRigidBodyCollision(false);
	pMesh->SetGenerateOverlapEvents(false);

	pUIComponent->AttachToComponent(pMesh,FAttachmentTransformRules::KeepRelativeTransform);
	pUIComponent->SetHiddenInGame(false);

	if(!GetWorldTimerManager().IsTimerActive(StratagemTimeHandle))
		GetWorldTimerManager().SetTimer(StratagemTimeHandle, [this]() {TimeBroadcast(); }, 1.f,true);

	UTPSGameInstance* pInstance = Cast<UTPSGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (pInstance)
	{
		pInstance->StartSoundLocation(sound_key::Deploy, GetWorld(), GetActorLocation(), ESoundAttenuationType::SOUND_LOUD);
	}
}

void AStratagem::Drop()
{
	//DropDelay 초기화
	fDelayTime = fSkillTable.Delay / 1000.f;

	//탄두Drop될 범위
	fSkillTable.EffectRadius;
	//StrataGem 상공 X미터 지점에서
	//EffectRadius 내 범위를 Random값으로 한군데 지정해서
	//그쪽방향으로 Explode Projectile 발사
	FVector vStart = GetActorLocation();
	FVector vEnd = vStart;

	vStart.Z += 10000.f;
	vEnd.X += (rand() % (int)(fSkillTable.EffectRadius * 2.f)) - fSkillTable.EffectRadius;
	vEnd.Y += (rand() % (int)(fSkillTable.EffectRadius * 2.f)) - fSkillTable.EffectRadius;

	FVector vDir = (vEnd - vStart).GetSafeNormal();
	FActorSpawnParameters params;
	AProjectile* pProjectile = GetWorld()->SpawnActor<AProjectile>(AProjectile::StaticClass(), vStart, vDir.Rotation(), params);
	pProjectile->SetData(false, fSkillTable.Radius, fSkillTable.Damage,this);
	pProjectile->SetDirection(vDir, 300.f);
	if(fSkillTable.Radius < 500.f)
	pProjectile->SetActorScale3D(FVector(0.3f));
}

void AStratagem::EffectEnd()
{
	ChangeState(ESkillEffectType::SKILL_END);

	if (pNiagaraCom) pNiagaraCom->Deactivate();
	pUIComponent->SetHiddenInGame(true);

	fCountDown = fSkillTable.Countdown / 1000.f;
	fDelayTime = fSkillTable.Delay / 1000.f;
	fEffectTime = fSkillTable.EffectTime / 1000.f;
}

void AStratagem::Effect(float Deltatime)
{
	if(eGemeffecttype == ESkillEffectType::SKILL_DEPLOY ||
		eGemeffecttype == ESkillEffectType::SKILL_READY) return;

	switch (eGemeffecttype)
	{
		case ESkillEffectType::SKILL_COUNTDOWN : 
			fCountDown -= Deltatime;
			if(fCountDown <= 0.f)
				ChangeState(ESkillEffectType::SKILL_EFFECT);

		break;
		case ESkillEffectType::SKILL_EFFECT : 
			fEffectTime -= Deltatime;
			if(fEffectTime <= 0.f)
				EffectEnd();

			fDelayTime -= Deltatime;
			if(fDelayTime <= 0.f)
				Drop();
			
		break;
		case ESkillEffectType::SKILL_END :
			fCoolDown -= Deltatime;
			if(fCoolDown <= 0.f)
				ResetStratagem();
	}
}

void AStratagem::ChangeState(ESkillEffectType state)
{
	eGemeffecttype = state;
}

void AStratagem::UpdateSpline(TArray<FPredictProjectilePathPointData>& PathData)
{
	if (!IsValid(Spline_Path)) return;

	ClearSpline();

	for (auto& elem_Path : PathData)
	{
		Spline_Path->AddSplinePoint(elem_Path.Location, ESplineCoordinateSpace::World);
	}

	for (int32 i = 0; i < Spline_Path->GetNumberOfSplinePoints() - 1; ++i)
	{
		USplineMeshComponent* pMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
		pMeshComponent->SetForwardAxis(ESplineMeshAxis::Z);
		if (SplineMesh)
			pMeshComponent->SetStaticMesh(SplineMesh);
		pMeshComponent->SetMobility(EComponentMobility::Movable);
		pMeshComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		pMeshComponent->RegisterComponentWithWorld(GetWorld());
		pMeshComponent->AttachToComponent(Spline_Path, FAttachmentTransformRules::KeepRelativeTransform);
		pMeshComponent->SetStartScale(FVector2D(UKismetSystemLibrary::MakeLiteralFloat(0.1f), UKismetSystemLibrary::MakeLiteralFloat(0.1f)));
		pMeshComponent->SetEndScale(FVector2D(UKismetSystemLibrary::MakeLiteralFloat(0.1f), UKismetSystemLibrary::MakeLiteralFloat(0.1f)));

		const FVector StartPoint = Spline_Path->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		const FVector StartTangent = Spline_Path->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
		const FVector EndPoint = Spline_Path->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
		const FVector EndTangent = Spline_Path->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);

		pMeshComponent->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent, true);
		pMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//Material 셋
		if (SplineMaterial)
			pMeshComponent->SetMaterial(0, SplineMaterial);

		ta_Spline_Mesh.Add(pMeshComponent);
	}
}

void AStratagem::ClearSpline()
{
	if (!IsValid(Spline_Path)) return;

	Spline_Path->ClearSplinePoints(true);
	if (ta_Spline_Mesh.Num() > 0)
	{
		for (auto elem_Meshs : ta_Spline_Mesh)
		{
			if (IsValid(elem_Meshs))
				elem_Meshs->DestroyComponent();
		}
		ta_Spline_Mesh.Empty();
	}
}

void AStratagem::ResetStratagem()
{
	AttachToComponent(pOwnerPlayer->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("r_hand_rifle"));
	ChangeState(ESkillEffectType::SKILL_READY);

	fCoolDown = fSkillTable.Cooldown / 1000.f;
	if (pOwnerPlayer.IsValid())
		pOwnerPlayer->SkillCool(fSkillTable.Skillkey, 0.f);

	SetHide(true);
	pUIComponent->SetHiddenInGame(true);

	if(StratagemTimeHandle.IsValid())
		GetWorldTimerManager().ClearTimer(StratagemTimeHandle);
}

void AStratagem::TimeBroadcast()
{
	if(!func_Stratagem_Time.IsBound()) return;
	
	switch (eGemeffecttype)
	{
	case ESkillEffectType::SKILL_COUNTDOWN:
		func_Stratagem_Time.Broadcast(iPlayerSkillIdx, (int32)eGemeffecttype, fCountDown);
		break;
	case ESkillEffectType::SKILL_EFFECT:
		func_Stratagem_Time.Broadcast(iPlayerSkillIdx, (int32)eGemeffecttype, fEffectTime);
		break;
	case ESkillEffectType::SKILL_END:
		func_Stratagem_Time.Broadcast(iPlayerSkillIdx, (int32)eGemeffecttype, fCoolDown);
		if(pOwnerPlayer.IsValid())
			pOwnerPlayer->SkillCool(fSkillTable.Skillkey, fCoolDown);
		break;
	default : break;
	}
}

// Called when the game starts or when spawned
void AStratagem::BeginPlay()
{
	Super::BeginPlay();
	
	InitDelegate();

	if(IsValid(pUIComponent))
	pUIComponent->GetWidget()->SetRenderScale(FVector2D(0.5f));
}

void AStratagem::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	if(eGemeffecttype != ESkillEffectType::SKILL_DEPLOY) return;

	if(Other == pOwnerPlayer) return;

	Deployment();
}

void AStratagem::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (eGemeffecttype != ESkillEffectType::SKILL_DEPLOY) return;
	if (OtherActor == pOwnerPlayer) return;
	Deployment();
}

// Called every frame
void AStratagem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Effect( DeltaTime);
}

void AStratagem::SetData(FSkillTable* data)
{
	SetSkill(data);
	fCountDown = data->Countdown / 1000.f;
	fDelayTime = data->Delay / 1000.f;
	fEffectTime = data->EffectTime / 1000.f;
	fCoolDown = data->Cooldown / 1000.f;
}

void AStratagem::SetData(FSkillTable data)
{
	SetSkill(data);
	fCountDown = data.Countdown / 1000.f;
	fDelayTime = data.Delay / 1000.f;
	fEffectTime = data.EffectTime / 1000.f;
	fCoolDown = data.Cooldown / 1000.f;
}

void AStratagem::SetHide(bool ishide)
{
	SetActorHiddenInGame(ishide);
}

void AStratagem::SetCharacterOwner(ATPSPortfolioCharacter* powner, int32 owerskillidx)
{
	pOwnerPlayer = TWeakObjectPtr<ATPSPortfolioCharacter>(powner);
	iPlayerSkillIdx = owerskillidx;
}

bool AStratagem::IsReady()
{
	return eGemeffecttype == ESkillEffectType::SKILL_READY;
}

void AStratagem::ArcTrace()
{
	if(nullptr == pOwnerPlayer || !pOwnerPlayer.IsValid()) return;

	FPredictProjectilePathResult Result;
	FVector vLocation = pOwnerPlayer->GetMesh()->GetSocketLocation(FName(TEXT("l_hand_socket")));
	FVector vAimPos = pOwnerPlayer->GetAimPosVector();
	FVector vLaunchVector = (vAimPos - vLocation).GetSafeNormal();
	vLaunchVector = vLaunchVector.RotateAngleAxis(-30.f, pOwnerPlayer->GetControlVector(false));
	vLaunchVector *= 1000.f;
	FPredictProjectilePathParams Params(50.f, vLocation, vLaunchVector, 10.f, ECC_Visibility);
	Params.DrawDebugType == EDrawDebugTrace::ForOneFrame;
	Params.ActorsToIgnore.Add(this);
	Params.ActorsToIgnore.Add(GetOwner());

	if (UGameplayStatics::PredictProjectilePath(GetWorld(), Params, Result))
	{
		UpdateSpline(Result.PathData);
	}
}

void AStratagem::ArcAttack()
{
	pMesh->SetSimulatePhysics(true);
	pMesh->SetNotifyRigidBodyCollision(true);
	pMesh->SetGenerateOverlapEvents(true);
	DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);

	ChangeState(ESkillEffectType::SKILL_DEPLOY);

	ClearSpline();
	if (nullptr == pOwnerPlayer || !pOwnerPlayer.IsValid()) return;

	FVector vLocation = pOwnerPlayer->GetMesh()->GetSocketLocation(FName(TEXT("l_hand_socket")));
	FVector vAimPos = pOwnerPlayer->GetAimPosVector();
	FVector vLaunchVector = (vAimPos - vLocation).GetSafeNormal();
	vLaunchVector = vLaunchVector.RotateAngleAxis(-30.f, pOwnerPlayer->GetControlVector(false));
	vLaunchVector *= 1000.f;

	pMesh->AddImpulse(vLaunchVector, NAME_None, true);
}


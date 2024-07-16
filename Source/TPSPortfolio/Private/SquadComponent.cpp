// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadComponent.h"
#include "kismet/GameplayStatics.h"
#include "Enemy.h"
#include "Scouter.h"
#include "EnemyController.h"

// Sets default values for this component's properties
USquadComponent::USquadComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	ta_Squad={stSquadPos{nullptr, FVector(300.f, -300.f, 0.f)},
			  stSquadPos{nullptr, FVector(300.f, 300.f, 0.f)}, 
			  stSquadPos{nullptr, FVector(-300.f, -300.f, 0.f)}, 
			  stSquadPos{nullptr, FVector(-300.f, 300.f, 0.f)}};
}


bool USquadComponent::IsEmptySquadPos()
{
	for (auto& elem : ta_Squad)
	{
		if (!elem.pPosOwner.IsValid())
		{
			return true;
		}
	}

	return false;
}

bool USquadComponent::SetSquadPos(AActor* pactor)
{
	if(!IsValid(GetOwner())) return false;

	for (auto& elem : ta_Squad)
	{
		if (!elem.pPosOwner.IsValid())
		{
			elem.pPosOwner = TWeakObjectPtr<AEnemy>(Cast<AEnemy>(pactor));
			Cast<AEnemy>(pactor)->SetSquad(Cast<AEnemy>(GetOwner()));
			return true;
		}
	}

	return false;
}

void USquadComponent::UpdateSquadPos()
{
	if(!IsValid(GetOwner())) return;

	for (auto& elem : ta_Squad)
	{
		if (!elem.pPosOwner.IsValid()) continue;

		//Squad member에게 Patrol 포지션 지정
		elem.pPosOwner->SetSquadPos(elem.vOffset + GetOwner()->GetActorLocation());

	}
}

bool USquadComponent::IsMySquad(AActor* pactor)
{
	for (auto& elem : ta_Squad)
	{
		if (!elem.pPosOwner.IsValid()) continue;
		if(elem.pPosOwner == pactor) return true;
	}

	return false;
}

void USquadComponent::ReleaseSquadPos()
{
	if (GetOwner()->GetWorldTimerManager().IsTimerActive(FindSquadTimehandle))
		GetOwner()->GetWorldTimerManager().ClearTimer(FindSquadTimehandle);

	for (auto& elem : ta_Squad)
	{
		if (elem.pPosOwner.IsValid())
		{
			elem.pPosOwner->SetSquad(nullptr);
		}
	}
}

// Called when the game starts
void USquadComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if(!IsValid(GetOwner())) return;
	if(!GetOwner()->GetWorldTimerManager().IsTimerActive(FindSquadTimehandle))
		GetOwner()->GetWorldTimerManager().SetTimer(FindSquadTimehandle,[this](){FindSquadMember();}, 5.f,true);

}


// Called every frame
void USquadComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateSquadPos();
	// ...
}

void USquadComponent::FindSquadMember()
{
	if(!IsValid(GetOwner())) return;

	AActor* pOwner = GetOwner();
	if(pOwner->IsA(AScouter::StaticClass()) && Cast<AScouter>(pOwner)->GetEnemyState() == EEnemyState::ATTACK ) return;
	if(!IsEmptySquadPos()) return;

	TArray<AActor*> ta_WorldActor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), ta_WorldActor);
	FVector vMypos = GetOwner()->GetActorLocation();

	UE_LOG(LogTemp, Log, TEXT("GetAnemy : %i"), ta_WorldActor.Num());

	ta_WorldActor.Sort([vMypos](const AActor& A, const AActor& B){return (vMypos-A.GetActorLocation()).Length() < (vMypos - B.GetActorLocation()).Length();});


	for (auto& vec_elem : ta_WorldActor)
	{
		if(vec_elem->IsA(AScouter::StaticClass())) continue;
		if((vMypos - vec_elem->GetActorLocation()).Length() > 3000.f) continue;
		if (Cast<AEnemy>(vec_elem)->HasSquad())continue;
		
		if(!SetSquadPos(vec_elem))
			break;
	}
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "BTFindPatrolPos.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTFindPatrolPos::UBTFindPatrolPos()
{
	NodeName = TEXT("FindPatrolPos");
}

EBTNodeResult::Type UBTFindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp,NodeMemory);

	APawn* pOwnerPawn = OwnerComp.GetAIOwner()->GetPawn();
	if(!IsValid(pOwnerPawn)) return EBTNodeResult::Failed;

	UNavigationSystemV1* pNavi = UNavigationSystemV1::GetNavigationSystem(pOwnerPawn->GetWorld());
	if(!IsValid(pNavi)) return EBTNodeResult::Failed;

	FVector vSrc = pOwnerPawn->GetActorLocation();
	FNavLocation vDst;

	if (pNavi->GetRandomPointInNavigableRadius(vSrc, 500.f, vDst))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(bb_key::PatrolLocation, vDst.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

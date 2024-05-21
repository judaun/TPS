// Fill out your copyright notice in the Description page of Project Settings.


#include "BTChangeState.h"
#include "TPSEnum.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Enemy.h"

UBTChangeState::UBTChangeState()
{
	NodeName = TEXT("ChangeState");
}

EBTNodeResult::Type UBTChangeState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp,NodeMemory);

	APawn* pAIPawn = OwnerComp.GetAIOwner()->GetPawn();
	AEnemy* pEnemy = Cast<AEnemy>(pAIPawn);
	if(!IsValid(pEnemy)) return EBTNodeResult::Failed;

	pEnemy->ChangeState(estate);

	return EBTNodeResult::Succeeded;
}

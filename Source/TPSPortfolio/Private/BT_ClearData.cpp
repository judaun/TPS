// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_ClearData.h"
#include "BehaviorTree/BlackboardComponent.h"

UBT_ClearData::UBT_ClearData() 
{
	NodeName=TEXT("ClearData");
}

EBTNodeResult::Type UBT_ClearData::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	OwnerComp.GetBlackboardComponent()->ClearValue(BlackboardKey.GetSelectedKeyID());
	return EBTNodeResult::Succeeded;
}
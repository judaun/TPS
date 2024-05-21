// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTFindPatrolPos.generated.h"

/**
 * 
 */

UCLASS()
class TPSPORTFOLIO_API UBTFindPatrolPos : public UBTTaskNode
{
	GENERATED_BODY()
	
	UBTFindPatrolPos();
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

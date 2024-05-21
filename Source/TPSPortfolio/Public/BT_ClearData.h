// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BT_ClearData.generated.h"

/**
 * 
 */
UCLASS()
class TPSPORTFOLIO_API UBT_ClearData : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	UBT_ClearData();
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

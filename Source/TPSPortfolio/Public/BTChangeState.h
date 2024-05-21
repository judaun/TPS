// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTChangeState.generated.h"

/**
 * 
 */
enum class EEnemyState : uint8;

UCLASS()
class TPSPORTFOLIO_API UBTChangeState : public UBTTaskNode
{
	GENERATED_BODY()
	UBTChangeState();

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(Category = Node, EditAnywhere)
	TEnumAsByte<EEnemyState> estate;
};

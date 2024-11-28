// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "MassAI/MassEnemy/Killed/Fragments/MassKilledParamsFragment.h"

#include "MassEnemyTrait.generated.h"
/**
 * 
 */
UCLASS()
class TPSPORTFOLIO_API UMassEnemyTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()
	
protected:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;

	UPROPERTY(Category="Killed", EditAnywhere)
	FMassEnemyKilledParamsFragment EnemyKilledParams;
};

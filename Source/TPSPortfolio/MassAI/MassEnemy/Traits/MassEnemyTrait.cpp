// Fill out your copyright notice in the Description page of Project Settings.


#include "MassAI/MassEnemy/Traits/MassEnemyTrait.h"

#include "MassEntityTemplateRegistry.h"
#include "MassAI/MassEnemy/Tags/MassEnemyTag.h"

void UMassEnemyTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);

	BuildContext.AddTag<FMassEnemyTag>();

	const FConstSharedStruct MassEnemyKilledParamsFragment = EntityManager.GetOrCreateConstSharedFragment(EnemyKilledParams);
	BuildContext.AddConstSharedFragment(MassEnemyKilledParamsFragment);
}

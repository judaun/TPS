// Fill out your copyright notice in the Description page of Project Settings.


#include "MassAI/MassEnemy/Killed/Observers/MassKilledObserver.h"

#include "MassCommonFragments.h"
#include "MassCommonTypes.h"
#include "MassExecutionContext.h"
#include "Kismet/GameplayStatics.h"
#include "MassAI/MassEnemy/Killed/Fragments/MassKilledParamsFragment.h"
#include "MassAI/MassEnemy/Killed/Tags/MassKilledTag.h"
#include "MassAI/MassEnemy/Tags/MassEnemyTag.h"

UMassKilledObserver::UMassKilledObserver()
{
	ObservedType = FMassKilledTag::StaticStruct();

	Operation = EMassObservedOperation::Add;

	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Movement);
}

void UMassKilledObserver::ConfigureQueries()
{
	MassEntityQuery.AddTagRequirement<FMassKilledTag>(EMassFragmentPresence::All);
	MassEntityQuery.AddTagRequirement<FMassEnemyTag>(EMassFragmentPresence::All);
	MassEntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	MassEntityQuery.AddConstSharedRequirement<FMassEnemyKilledParamsFragment>();

	MassEntityQuery.RegisterWithProcessor(*this);
}

void UMassKilledObserver::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	MassEntityQuery.ForEachEntityChunk(EntityManager,Context,[&](FMassExecutionContext& MassExecutionContext){
		const TConstArrayView<FTransformFragment> TransformList = MassExecutionContext.GetFragmentView<FTransformFragment>();
		const FMassEnemyKilledParamsFragment& MassEnemyKilledParams = MassExecutionContext.GetConstSharedFragment<FMassEnemyKilledParamsFragment>();

		for(int32 iEntityIdx = 0; iEntityIdx<MassExecutionContext.GetNumEntities(); ++iEntityIdx)
		{
			const FTransform EntityTransform = TransformList[iEntityIdx].GetTransform();

			const FVector ParticlePlayLoc = EntityTransform.GetLocation() + MassEnemyKilledParams.vParticlePlayOffset;

			UGameplayStatics::SpawnEmitterAtLocation(MassExecutionContext.GetWorld(), MassEnemyKilledParams.pParticleToPlayWhenKilled, ParticlePlayLoc);

		}

		Context.Defer().DestroyEntities(Context.GetEntities());
	});
}

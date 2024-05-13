// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_MorphTarget.h"

#if WITH_EDITOR
#include "Logging/MessageLog.h"
#include "Misc/UObjectToken.h"
#endif

UAnimNotify_MorphTarget::UAnimNotify_MorphTarget()
	: Super()
{
	bTickable = true;
	bTickableWhenPaused = false;
	StartPoint = 0.f;
	EndPoint = 0.f;
	ReturningCount= 0;
	DurationTime = 1.f;
	Playrate = 1.f;
	fMorphValue = 0.f;
	fReturn = false;

	ConstructorHelpers::FObjectFinder<UCurveFloat> FObj_Curve(TEXT("/Script/Engine.CurveFloat'/Game/ThirdPerson/Curves/floatCurveBase.floatCurveBase'"));
	if (FObj_Curve.Succeeded())
	{
		MorphCurve = FObj_Curve.Object;
		UE_LOG(LogTemp, Log, TEXT("FObj_Curve is Succeeded"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("FObj_Curve is Fail"));
	}

	InitTimeLine();
}

void UAnimNotify_MorphTarget::Tick(float DeltaTime)
{
	MorphTimeline.TickTimeline(DeltaTime);
}

bool UAnimNotify_MorphTarget::IsTickable() const
{
	return bTickable;
}

bool UAnimNotify_MorphTarget::IsTickableInEditor() const
{
	return bTickable;
}

bool UAnimNotify_MorphTarget::IsTickableWhenPaused() const
{
	return bTickableWhenPaused;
}

TStatId UAnimNotify_MorphTarget::GetStatId() const
{
	return TStatId();
}

UWorld* UAnimNotify_MorphTarget::GetWorld() const
{
	return GetOuter()->GetWorld();
}

void UAnimNotify_MorphTarget::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{

}

void UAnimNotify_MorphTarget::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	Notify(MeshComp, Animation);
	PRAGMA_ENABLE_DEPRECATION_WARNINGS

	if (!MorphTargetName.IsNone() && MeshComp)
	{
		pMeshComp = MeshComp;
		RefreshTimeLine();
		MorphTimeline.PlayFromStart();
	}
}

void UAnimNotify_MorphTarget::ValidateAssociatedAssets()
{
	static const FName NAME_AssetCheck("AssetCheck");

	if (MorphTargetName.IsNone())
	{
		if (GIsEditor)
		{
			UObject* ContainingAsset = GetContainingAsset();

			FMessageLog AssetCheckLog(NAME_AssetCheck);
			const FText MessageLooping = NSLOCTEXT("AnimNotify", "MorphTarget", "MorphTargetNameNone.");
			AssetCheckLog.Warning()
				->AddToken(FUObjectToken::Create(ContainingAsset))
				->AddToken(FTextToken::Create(MessageLooping));

			AssetCheckLog.Notify(MessageLooping, EMessageSeverity::Warning, true);
		}
	}
}

void UAnimNotify_MorphTarget::OnMorphProgress(float morphvalue)
{
	if (MorphTargetName.IsNone())
	{
		return;
	}
	if (nullptr == pMeshComp || !IsValid(pMeshComp))
	{
		return;
	}
	if (!fReturn)
		fMorphValue = morphvalue;
	else
		fMorphValue = EndPoint - morphvalue;
	
	fMorphValue = FMath::Max(StartPoint, FMath::Min(EndPoint, fMorphValue));

	pMeshComp->SetMorphTarget(MorphTargetName, fMorphValue);
}

void UAnimNotify_MorphTarget::OnMorphTimelineFinish()
{
	if (iReturncnt <= 0) return;

	fReturn = !fReturn;
	--iReturncnt;
	MorphTimeline.PlayFromStart();
}

void UAnimNotify_MorphTarget::InitTimeLine()
{
	if (nullptr == MorphCurve) return;

	FOnTimelineFloat MorphTimelineCallback;
	FOnTimelineEventStatic MorphTimelineFinishCallback;

	MorphTimelineCallback.BindUFunction(this, FName("OnMorphProgress"));
	MorphTimelineFinishCallback.BindUFunction(this, FName("OnMorphTimelineFinish"));

	MorphTimeline.AddInterpFloat(MorphCurve, MorphTimelineCallback);
	MorphTimeline.SetTimelineFinishedFunc(MorphTimelineFinishCallback);
	MorphTimeline.SetTimelineLengthMode(ETimelineLengthMode::TL_TimelineLength);
	MorphTimeline.SetTimelineLength(DurationTime);
	MorphTimeline.SetPlayRate(Playrate);

	UE_LOG(LogTemp, Log, TEXT("MorphTimeline is Init"));
}

void UAnimNotify_MorphTarget::RefreshTimeLine()
{
	iReturncnt = ReturningCount;
	fReturn = false;
	MorphTimeline.SetTimelineLength(DurationTime);
	MorphTimeline.SetPlayRate(Playrate);
}

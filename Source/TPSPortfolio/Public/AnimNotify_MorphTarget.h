// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Tickable.h"
#include "Components/TimeLineComponent.h"
#include "AnimNotify_MorphTarget.generated.h"

/**
 * 
 */

UCLASS(const, hidecategories = Object, collapsecategories, Config = Game, meta = (DisplayName = "Morph Target"))
class TPSPORTFOLIO_API UAnimNotify_MorphTarget : public UAnimNotify , public FTickableGameObject
{
	GENERATED_BODY()
	
public:
	UAnimNotify_MorphTarget();
	virtual void Tick(float DeltaTime) override;
	/** Tick을 활성화할지 여부를 반환하는 함수입니다. */
	virtual bool IsTickable() const override;
	/** 에디터에서 Tick을 활성화할지 여부를 반환하는 함수입니다. 일반적으로 true를 반환하여 에디터에서도 객체의 Tick을 활성화할 수 있도록 설정합니다. */
	virtual bool IsTickableInEditor() const override;
	/** 게임이 일시 정지되었을 때 Tick을 계속해서 호출할지 여부를 반환하는 함수입니다. */
	virtual bool IsTickableWhenPaused() const override;
	/** 객체의 고유한 통계 식별자를 반환하는 함수입니다. */
	virtual TStatId GetStatId() const override;
	/** 객체가 존재하는 World를 반환하는 함수입니다. */
	virtual UWorld* GetWorld() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual void ValidateAssociatedAssets() override;

	UFUNCTION()
		void OnMorphProgress(float morphvalue);

	UFUNCTION()
		void OnMorphTimelineFinish();

private:
	void InitTimeLine();
	void RefreshTimeLine();
protected:
	/** Tick 함수의 활성화/비활성화를 설정하는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tick")
		bool bTickable;

	/** 게임이 일시 정지되었을 때 Tick 함수를 호출할지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tick")
		bool bTickableWhenPaused;

public:
	/** 모프타겟으로 지정될 이름입니다.  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	FName MorphTargetName;
	/** 타임라인 재생 시간입니다 기본 1입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	float DurationTime;
	/** 타임라인 재생 배율입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	float Playrate;
	/** 모프타겟의 최소값입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	float StartPoint;
	/** 모프타겟의 최대값입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	float EndPoint;
	/** 모프타겟 반복 횟수입니다 리버스로 동작합니다 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	int32 ReturningCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Timeline", Meta = (AllowPrivateAccess = "true"))
	UCurveFloat* MorphCurve;
private:
	UPROPERTY()
	FTimeline MorphTimeline;

	UPROPERTY()
	USkeletalMeshComponent* pMeshComp;

	float fMorphValue;
	bool fReturn;
	int32 iReturncnt;
};

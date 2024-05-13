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
	/** Tick�� Ȱ��ȭ���� ���θ� ��ȯ�ϴ� �Լ��Դϴ�. */
	virtual bool IsTickable() const override;
	/** �����Ϳ��� Tick�� Ȱ��ȭ���� ���θ� ��ȯ�ϴ� �Լ��Դϴ�. �Ϲ������� true�� ��ȯ�Ͽ� �����Ϳ����� ��ü�� Tick�� Ȱ��ȭ�� �� �ֵ��� �����մϴ�. */
	virtual bool IsTickableInEditor() const override;
	/** ������ �Ͻ� �����Ǿ��� �� Tick�� ����ؼ� ȣ������ ���θ� ��ȯ�ϴ� �Լ��Դϴ�. */
	virtual bool IsTickableWhenPaused() const override;
	/** ��ü�� ������ ��� �ĺ��ڸ� ��ȯ�ϴ� �Լ��Դϴ�. */
	virtual TStatId GetStatId() const override;
	/** ��ü�� �����ϴ� World�� ��ȯ�ϴ� �Լ��Դϴ�. */
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
	/** Tick �Լ��� Ȱ��ȭ/��Ȱ��ȭ�� �����ϴ� �����Դϴ�. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tick")
		bool bTickable;

	/** ������ �Ͻ� �����Ǿ��� �� Tick �Լ��� ȣ������ ��Ÿ���� �����Դϴ�. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tick")
		bool bTickableWhenPaused;

public:
	/** ����Ÿ������ ������ �̸��Դϴ�.  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	FName MorphTargetName;
	/** Ÿ�Ӷ��� ��� �ð��Դϴ� �⺻ 1�Դϴ�. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	float DurationTime;
	/** Ÿ�Ӷ��� ��� �����Դϴ�. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	float Playrate;
	/** ����Ÿ���� �ּҰ��Դϴ�. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	float StartPoint;
	/** ����Ÿ���� �ִ밪�Դϴ�. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	float EndPoint;
	/** ����Ÿ�� �ݺ� Ƚ���Դϴ� �������� �����մϴ� */
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

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuadFootIK.generated.h"

struct FQuadIKData
{
	FName BoneName;
	FName SocketName;
	float fFootOffset = 0.f;
	float fWorldOffset = 0.f;
	FVector vLocation = FVector::ZeroVector;
	FVector vDstLocation = FVector::ZeroVector;
	float fMoving = 0.f;
	float fCurveData = 0.f;
};

#define MAXDISTANCE 50

class UCurveVector;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPSPORTFOLIO_API UQuadFootIK : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UQuadFootIK();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	/* �ٸ� �� �̸��� ��Ĺ �̸��� �������� �Լ� */
	void SetLegData(bool left, bool front, FName bonename, FName socketname);
private:
	/* �ٸ� ������ ����Ʈ���̽��� ���� �ٴ��� ���� ����Ʈ�� �̾Ƴ��� �Լ� */
	void FootIKTrace(float tracedistance, FQuadIKData* ikdata);
	/* ���� ĸ�� ���� ���(���� ���� ������) */
	void UpdateCapsuleHeight(float Deltatime, float hipoffset);
	/* ������ ���� ���� �Լ� */
	void UpdateFootOffset(float Deltatime, float targetvalue, float* effvalue, float interpspeed);
	/* �ٸ� ��ġ���� ���� ���� �Լ� Ŀ�굥���ͷ� ���� ���� */
	void UpdateVectorOffset(float Deltatime, FVector targetvalue, FVector* effvalue, float* curvevalue, float interpspeed);
	/* �� �̸����� �ٸ� ��ġ ��� */
	void UpdateFootLocation();
	/* ������ ������� ���� ��ġ�� Roll */
	void UpdateBodyRoll(float DeltaTime);
	/* �ٸ� �� �̸� ��Ĺ �̸��� ����ü�� �Ҵ� */
	void SetLegData(FQuadIKData* ikdata, FName bonename, FName socketname);
public:
	UFUNCTION(BlueprintCallable, Category = FootIK)
		float GetHipOffset() { return fHipOffset; }

	UFUNCTION(BlueprintCallable, Category = FootIK)
		FVector GetFootVectorOffset(bool left, bool front);

	UFUNCTION(BlueprintCallable, Category = FootIK)
		FRotator GetBodyRotator() { return rotBody; }
private:
	float fHipOffset;
	float fHalfCapsuleHeight;

	TArray<FQuadIKData> TA_IKData;

	UPROPERTY()
		UCurveVector* MoveCurve;

	FVector vDstFrameLocation;
	FVector vSrcFrameLocation;
	FRotator rotBody;
};

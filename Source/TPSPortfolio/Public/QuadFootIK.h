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
	/* 다리 본 이름과 소캣 이름을 지정해줄 함수 */
	void SetLegData(bool left, bool front, FName bonename, FName socketname);
private:
	/* 다리 본에서 라인트레이스를 통해 바닥의 딛을 포인트를 뽑아내는 함수 */
	void FootIKTrace(float tracedistance, FQuadIKData* ikdata);
	/* 현재 캡슐 높이 계산(몸통 높이 보정용) */
	void UpdateCapsuleHeight(float Deltatime, float hipoffset);
	/* 오프셋 보간 계산용 함수 */
	void UpdateFootOffset(float Deltatime, float targetvalue, float* effvalue, float interpspeed);
	/* 다리 위치벡터 보간 계산용 함수 커브데이터로 높이 조절 */
	void UpdateVectorOffset(float Deltatime, FVector targetvalue, FVector* effvalue, float* curvevalue, float interpspeed);
	/* 본 이름으로 다리 위치 계산 */
	void UpdateFootLocation();
	/* 높낮이 기반으로 몸통 위치를 Roll */
	void UpdateBodyRoll(float DeltaTime);
	/* 다리 본 이름 소캣 이름을 구조체에 할당 */
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

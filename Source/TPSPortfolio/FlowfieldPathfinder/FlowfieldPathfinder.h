// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FlowfieldPathfinder.generated.h"

/**
 * 
 */
 class AFlowfieldVolume;
 class UFlowCell;

UCLASS()
class TPSPORTFOLIO_API UFlowfieldPathfinder : public UObject
{
	GENERATED_BODY()

	~UFlowfieldPathfinder();

private:
	/*현재 위치 기반으로 FlowCell찾기*/
	UFlowCell* FindFlowCell(FTransform& current);
	/*FlowFieldPathFinder의 데이터 전부 제거*/
	void ClearMap();
	/*Volume을 이용해 Cell 생성*/
	void CreateCell(AFlowfieldVolume* fieldvolume, UWorld* InWorld, float distance);
	/*길찾기 전 이전기록 데이터 전부 초기화*/
	void ResetFlowData();

	/*스탭 수 target 기준으로 계산*/
	void StartCalcSteps(UFlowCell* targetcell);

	void CalcMapSteps(TMap<int32, TArray<TTuple<int32, int32>>>& map, int32 curstep, int32 index_x, int32 index_y);

	/*보관하고있는 Cell의 방향 계산*/
	void CalcDirection();
	/*주변 Cell을 확인해서 최소Step Cell을 반환*/
	UFlowCell* GetMinStepCell(int32 index_x, int32 index_y);
	/*각 Cell의 Step수 반환*/
	UFlowCell* GetStepCell(int32 index_x, int32 index_y);
public:
	/*현재 위치 기반으로 FlowCell찾기*/
	UFlowCell* FindNearCell(FTransform& current);
	/*Access가능한 FlowCell만 탐지(9방향)*/
	UFlowCell* FindNearAccessCell(FTransform& current);

	/*현재 위치 기반으로 FlowDirection 가져오기*/
	FVector GetFlowDirection(FTransform& current);

	/*Volume에 따라서 FlowCell Update*/
	void UpdateFlowfield(AFlowfieldVolume* fieldvolume, UWorld* InWorld, float distance);

	/*FlowDirection 지정해주기*/
	void SetFlowDirections(FTransform& target);

	/*FlowDirection Debug View*/
	void ShowDebugDirection(UWorld* InWorld);
protected:

private:
	//UFlowCell 보관함
	TMap<TTuple<int32,int32>, TStrongObjectPtr<UFlowCell>> mapFlowCell;

	FVector vStartCell;
	FVector vEndCell;

	TWeakObjectPtr<UFlowCell> pCurTargetCell;

	float fDistance;

	int32 iMaxX;
	int32 iMaxY;
};

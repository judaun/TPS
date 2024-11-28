// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowfieldPathfinder.h"
#include "FlowfieldVolume.h"
#include "FlowCell.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

UFlowfieldPathfinder::~UFlowfieldPathfinder()
{
	ClearMap();
}

UFlowCell* UFlowfieldPathfinder::FindFlowCell(FTransform& current)
{
	FVector vCellPos = current.GetTranslation();
	
	if(vCellPos.X < vStartCell.X || vCellPos.Y < vStartCell.Y || 
		vCellPos.X > vEndCell.X || vCellPos.Y > vEndCell.Y) return nullptr;

	int32 iIndexX = round((vCellPos.X - vStartCell.X) / fDistance);
	int32 iIndexY = round((vCellPos.Y - vStartCell.Y) / fDistance);

	auto pCell = mapFlowCell.Find(TTuple<int32, int32>(iIndexX, iIndexY));
	if(pCell && pCell->IsValid())
		return pCell->Get();

	return nullptr;
}

void UFlowfieldPathfinder::ClearMap()
{
	if (!mapFlowCell.IsEmpty())
	{
		for (auto& elem : mapFlowCell)
		{
			if (elem.Value)
				elem.Value.Reset();
		}
		mapFlowCell.Empty();
	}
}

void UFlowfieldPathfinder::CreateCell(AFlowfieldVolume* fieldvolume, UWorld* InWorld, float distance)
{
	auto bounds = fieldvolume->GetBounds();

#pragma region CellValueInit
	vStartCell = FVector(bounds.Origin.X - bounds.BoxExtent.X, bounds.Origin.Y - bounds.BoxExtent.Y, bounds.Origin.Z);
	vEndCell = FVector(bounds.Origin.X + bounds.BoxExtent.X, bounds.Origin.Y + bounds.BoxExtent.Y, bounds.Origin.Z);
	fDistance = distance;
#pragma endregion

	FVector vDir_X = (FVector(vEndCell.X, 0.f, 0.f) - FVector(vStartCell.X, 0.f, 0.f)).GetSafeNormal();
	FVector vDir_Y = (FVector(0.f, vEndCell.Y, 0.f) - FVector(0.f, vStartCell.Y, 0.f)).GetSafeNormal();

	TArray<AActor*> IgnoreActor;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(InWorld);
	if (nullptr == NavSystem) return;

	FVector vCellPoint = vStartCell;

	iMaxX = (bounds.BoxExtent.X * 2.f)/distance;
	iMaxY = (bounds.BoxExtent.Y * 2.f)/distance;

	for (int iY = 0; iY <= iMaxY; ++iY)
	{
		for (int iX = 0; iX <= iMaxX; ++iX)
		{
			UFlowCell* pNewCell = NewObject<UFlowCell>(this, UFlowCell::StaticClass());

			FNavLocation OutNavLocation;
			if (NavSystem->ProjectPointToNavigation(vCellPoint, OutNavLocation, FVector(1.f, 1.f, bounds.BoxExtent.Z)))
			{
				FVector vTraceStart = FVector(OutNavLocation.Location.X, OutNavLocation.Location.Y, bounds.Origin.Z + bounds.BoxExtent.Z);
				FVector vTraceEnd = vTraceStart + (FVector::UpVector * -(bounds.BoxExtent.Z * 2.f));

				FHitResult hitResult;

				if (UKismetSystemLibrary::LineTraceSingle(InWorld, vTraceStart, vTraceEnd,
					UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_WorldStatic),
					true, IgnoreActor, EDrawDebugTrace::None, hitResult, true))
				{
					pNewCell->SetPos(hitResult.Location, distance);
#if WITH_EDITOR
					if (!GIsPlayInEditorWorld)
					{
						DrawDebugBox(InWorld, hitResult.Location, FVector(3.f), FColor::Cyan, false, 5.f, 0, 3.f);
					}
#endif
				}
				else
					pNewCell->SetPos(OutNavLocation.Location, distance);

				pNewCell->UpdateCellAccess(true);
			}
			else
			{
				pNewCell->SetPos(vCellPoint, distance);
#if WITH_EDITOR
				if (!GIsPlayInEditorWorld)
				{
					DrawDebugBox(InWorld, vCellPoint, FVector(distance / 2.f, distance / 2.f, bounds.BoxExtent.Z), FColor::Red, false, 5.f, 0, 3.f);
				}
#endif
			}

			TStrongObjectPtr<UFlowCell> pStrongCell = TStrongObjectPtr<UFlowCell>(pNewCell);
			mapFlowCell.Add(TTuple<int, int>(iX, iY), pStrongCell);

			vCellPoint += vDir_X * distance;
		}
		vCellPoint.X = vStartCell.X;
		vCellPoint += vDir_Y * distance;
	}

	UE_LOG(LogTemp, Warning, TEXT("UpdateFlowfield!"));
	UE_LOG(LogTemp, Warning, TEXT("%d Cell Created"), mapFlowCell.Num());

}

void UFlowfieldPathfinder::ResetFlowData()
{
	if (!mapFlowCell.IsEmpty())
	{
		for (auto& elem : mapFlowCell)
		{
			if (elem.Value)
			{
				elem.Value->SetStep(INT32_MAX, false);
			}	
		}
	}
}

void UFlowfieldPathfinder::StartCalcSteps(UFlowCell* targetcell)
{
	if (nullptr == targetcell)
	{
		UE_LOG(LogTemp, Warning, TEXT("TargetCell is null!"));
		return;
	}


	TMap<int32, TArray<TTuple<int32,int32>>> mapSteps;
	int32 iStepIndex = 0;

	TArray<TTuple<int32, int32>> arrIndex;

	FVector vCellPos = targetcell->GetPos();
	int32 iIndexX = round((vCellPos.X - vStartCell.X) / fDistance);
	int32 iIndexY = round((vCellPos.Y - vStartCell.Y) / fDistance);

	arrIndex.Emplace(TTuple<int32, int32>(iIndexX, iIndexY));
	mapSteps.Emplace(iStepIndex, arrIndex);

	while (TArray<TTuple<int32, int32>>* pArr = mapSteps.Find(iStepIndex))
	{
		for (auto& elem : *pArr)
		{
			CalcMapSteps(mapSteps, iStepIndex, elem.Key, elem.Value);
		}

		++iStepIndex;
	}

	mapSteps.Reset();
}

void UFlowfieldPathfinder::CalcMapSteps(TMap<int32, TArray<TTuple<int32, int32>>>& map, int32 curstep, int32 index_x, int32 index_y)
{
	if (index_x < 0 || index_y < 0 ||
		index_x > iMaxX || index_y > iMaxY)
		return;

	auto pCell = mapFlowCell.Find(TTuple<int32, int32>(index_x, index_y));
	if (pCell == nullptr || !pCell->IsValid()) return;
	if (pCell->Get()->IsCheckedCell()) return;

	pCell->Get()->SetStep(curstep, true);

	TArray<TTuple<int32, int32>>* pArr = map.Find(curstep+1);
	if (nullptr == pArr)
	{
		TArray<TTuple<int32, int32>> arrIndex;
		arrIndex.Emplace(TTuple<int32, int32>(index_x - 1, index_y));
		arrIndex.Emplace(TTuple<int32, int32>(index_x, index_y + 1));
		arrIndex.Emplace(TTuple<int32, int32>(index_x + 1, index_y));
		arrIndex.Emplace(TTuple<int32, int32>(index_x, index_y - 1));

		map.Emplace(curstep + 1, arrIndex);
	}
	else
	{
		pArr->Emplace(TTuple<int32, int32>(index_x - 1, index_y));
		pArr->Emplace(TTuple<int32, int32>(index_x, index_y + 1));
		pArr->Emplace(TTuple<int32, int32>(index_x + 1, index_y));
		pArr->Emplace(TTuple<int32, int32>(index_x, index_y - 1));
	}
}

void UFlowfieldPathfinder::CalcDirection()
{
	if (mapFlowCell.IsEmpty()) return;

	for (auto& elem : mapFlowCell)
	{
		if (elem.Value)
		{
			auto pCalcCell = elem.Value.Get();

			if(!pCalcCell->IsAccess()) continue;
			if (pCalcCell->GetStep() == 0) 
			{
				pCalcCell->UpdateDirection(FVector::UpVector);
				continue;
			}

			int32 iIndex_X = elem.Key.Key;
			int32 iIndex_Y = elem.Key.Value;
			//9방향체크
			auto pMinStepCell = GetMinStepCell(iIndex_X,iIndex_Y);
			if(nullptr == pMinStepCell) continue;

			FVector vDir = (pMinStepCell->GetPos() - pCalcCell->GetPos()).GetSafeNormal();
			pCalcCell->UpdateDirection(vDir);
		}
	}
}

UFlowCell* UFlowfieldPathfinder::GetMinStepCell(int32 index_x, int32 index_y)
{
	int32 iMinStep = INT32_MAX;
	UFlowCell* pMinStepCell = nullptr;

	for (int32 i = -1; i < 2; ++i)
	{
		for (int32 j = -1; j < 2; ++j)
		{
			if(i==0 && j==0) continue;

			// -1 -1 ~~~ 1 1 9방향으로 Cell 찾기
			UFlowCell* pCell = GetStepCell(index_x + i, index_y + j);
			if(nullptr == pCell) continue;
			if(!pCell->IsAccess()) continue;

			//최소Step찾기 찾으면 최소값 갱신 및 반환용 포인터에 할당
			int32 iCurCellStep = pCell->GetStep();
			if (iMinStep > iCurCellStep)
			{
				pMinStepCell = pCell;
				iMinStep = iCurCellStep;
			}
		}
	}

	return pMinStepCell;
}

UFlowCell* UFlowfieldPathfinder::GetStepCell(int32 index_x, int32 index_y)
{
	auto pCell = mapFlowCell.Find(TTuple<int32, int32>(index_x, index_y));
	if (pCell == nullptr || !pCell->IsValid()) return nullptr;
	if (!pCell->Get()->IsAccess()) return nullptr;

	return pCell->Get();
}

UFlowCell* UFlowfieldPathfinder::FindNearCell(FTransform& current)
{
	FVector vCellPos = current.GetTranslation();

	if(vCellPos.X < vStartCell.X) 
	if (vCellPos.X < vStartCell.X || vCellPos.Y < vStartCell.Y ||
		vCellPos.X > vEndCell.X || vCellPos.Y > vEndCell.Y) return nullptr;

	int32 iIndexX = round((vCellPos.X - vStartCell.X) / fDistance);
	int32 iIndexY = round((vCellPos.Y - vStartCell.Y) / fDistance);

	auto pCell = mapFlowCell.Find(TTuple<int32, int32>(iIndexX, iIndexY));
	if(nullptr == pCell || !pCell->IsValid()) return nullptr;

	return pCell->Get();
}

UFlowCell* UFlowfieldPathfinder::FindNearAccessCell(FTransform& current)
{
	FVector vCellPos = current.GetTranslation();

	if (vCellPos.X < vStartCell.X)
		if (vCellPos.X < vStartCell.X || vCellPos.Y < vStartCell.Y ||
			vCellPos.X > vEndCell.X || vCellPos.Y > vEndCell.Y) return nullptr;

	int32 iIndexX = round((vCellPos.X - vStartCell.X) / fDistance);
	int32 iIndexY = round((vCellPos.Y - vStartCell.Y) / fDistance);

	UFlowCell* pNearCell = nullptr;
	float fMinDist = INT32_MAX;

	for (int32 i = -1; i < 2; ++i)
	{
		for (int32 j = -1; j < 2; ++j)
		{
			if(i == 0 && j == 0) continue;

			auto pFindCell = mapFlowCell.Find(TTuple<int32, int32>(iIndexX + i, iIndexY + j));
			if (nullptr == pFindCell || !pFindCell->IsValid()) continue;
			auto pCell = pFindCell->Get();
			if(!pCell->IsAccess()) continue;

			float fDist = FVector::Dist(pCell->GetPos(), vCellPos);
			if (fDist < fMinDist)
			{
				fMinDist = fDist;
				pNearCell = pCell;
			}
		}
	}
	return pNearCell;
}

FVector UFlowfieldPathfinder::GetFlowDirection(FTransform& current)
{
	UFlowCell* pCell = FindFlowCell(current);
	if(nullptr == pCell) return FVector(0.f);
	return pCell->GetDirection();
}

void UFlowfieldPathfinder::UpdateFlowfield(AFlowfieldVolume* fieldvolume, UWorld* InWorld, float distance)
{
	//Volume 업데이트되어 크기 구성이 달라지면 Cell의 양도 달라짐 존재할경우 제거
	ClearMap();

	//Volume Bounds를 이용해 Cell 생성
	CreateCell(fieldvolume,InWorld,distance);
}

void UFlowfieldPathfinder::SetFlowDirections(FTransform& target)
{
	//Target으로부터 Cell 구해오기
	UFlowCell* pTargetCell = FindFlowCell(target);
	if(nullptr == pTargetCell) return;
	if(pCurTargetCell == pTargetCell) return;

	pCurTargetCell = pTargetCell;

	//Step수 초기화 및 방향 초기화
	ResetFlowData();

	//target기준으로 인접Cell의 Step계산 및 Chedk
	StartCalcSteps(pTargetCell);

	//방향벡터 생성
	CalcDirection();
}

void UFlowfieldPathfinder::ShowDebugDirection(UWorld* InWorld)
{
	for (auto& elem : mapFlowCell)
	{
		if (elem.Value)
		{
			auto pCell = elem.Value.Get();
			if(!pCell->IsAccess()) continue;
			if (pCell->GetDirection() == FVector::UpVector)
			{
				DrawDebugBox(InWorld,pCell->GetPos(), FVector(3.f),FColor::Red,false,5.f,0,3.f);
				continue;
			}
			FVector vEndPoint = pCell->GetPos() + (pCell->GetDirection() * (fDistance/2.f));
			UKismetSystemLibrary::DrawDebugArrow(InWorld, pCell->GetPos(), vEndPoint,5.f,FLinearColor::Green,3.f,5.f);
	
		}
	}
}

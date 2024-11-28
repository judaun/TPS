// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/TPSGameInstanceSubsystem.h"
#include "Operations/MeshBoolean.h"
#include "DynamicMesh/Operations/MergeCoincidentMeshEdges.h"
#include "MeshDescriptionToDynamicMesh.h"
#include "DynamicMeshToMeshDescription.h"
#include "UDynamicMesh.h"
#include "DynamicMeshEditor.h"
#include "MeshSimplification.h"

UTPSGameInstanceSubsystem::UTPSGameInstanceSubsystem()
{
	dyMesh_Boolean = CreateDefaultSubobject<UDynamicMeshComponent>(TEXT("BooleanMesh"));
	
	ConstructorHelpers::FObjectFinder<UMaterialInstance> FObj_findmaterial(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Effects/M_Broken_Inst.M_Broken_Inst'"));
	if (FObj_findmaterial.Succeeded())
	{
		BooleanMaterial = FObj_findmaterial.Object;
		dyMesh_Boolean->SetMaterial(0, BooleanMaterial);
	}

	
	ConstructorHelpers::FClassFinder<AActor> FCls_FindRVT(TEXT("Blueprint'/Game/ThirdPerson/Blueprints/BP_RVTEffect.BP_RVTEffect_C'"));
	if (FCls_FindRVT.Succeeded())
	{
		clsRVTEffect = FCls_FindRVT.Class;
	}
}

void UTPSGameInstanceSubsystem::CreateBooleanSphereAsync(float Radius, int32 LatitudeSegments, int32 LongitudeSegments)
{
	Async(EAsyncExecution::Thread, [this, Radius, LatitudeSegments, LongitudeSegments](){
		FDynamicMesh3 Mesh;

		// 위도와 경도에 따라 정점 추가
		for (int32 Latitude = 0; Latitude <= LatitudeSegments; Latitude++)
		{
			float Theta = FMath::DegreesToRadians(180.0f * Latitude / LatitudeSegments);
			float SinTheta = FMath::Sin(Theta);
			float CosTheta = FMath::Cos(Theta);

			for (int32 Longitude = 0; Longitude <= LongitudeSegments; Longitude++)
			{
				float Phi = FMath::DegreesToRadians(360.0f * Longitude / LongitudeSegments);
				float SinPhi = FMath::Sin(Phi);
				float CosPhi = FMath::Cos(Phi);

				// 정점 위치 계산
				FVector3d Vertex(
					Radius * SinTheta * CosPhi,
					Radius * SinTheta * SinPhi,
					Radius * CosTheta
				);
				Mesh.AppendVertex(Vertex);
			}
		}

		// 삼각형 추가 (위도와 경도에 따른 인덱스 계산)
		for (int32 Latitude = 0; Latitude < LatitudeSegments; Latitude++)
		{
			for (int32 Longitude = 0; Longitude < LongitudeSegments; Longitude++)
			{
				int32 Current = Latitude * (LongitudeSegments + 1) + Longitude;
				int32 Next = Current + LongitudeSegments + 1;

				// 삼각형 추가 (CW 순서)
				Mesh.AppendTriangle(Current, Current + 1, Next);    // 반대 방향 삼각형
				Mesh.AppendTriangle(Current + 1, Next + 1, Next);  // 반대 방향 삼각형
			}
		}

		AsyncTask(ENamedThreads::GameThread, [this, CreatedMesh = MoveTemp(Mesh)](){
			OnSphereCreated(CreatedMesh);
		});
	});
}

void UTPSGameInstanceSubsystem::OnSphereCreated(FDynamicMesh3 CreatedMesh)
{
	dyMesh_Boolean->SetMesh(MoveTemp(CreatedMesh));

}

void UTPSGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	CreateBooleanSphereAsync(1,5,5);
}

void UTPSGameInstanceSubsystem::Deinitialize()
{

}

UDynamicMeshComponent* UTPSGameInstanceSubsystem::GetBooleanMesh()
{
	return dyMesh_Boolean;
}

void UTPSGameInstanceSubsystem::MeshBoolean(UDynamicMeshComponent* srcMesh, FVector hitLocation, FVector booleanScale)
{
	Async(EAsyncExecution::Thread, [this, srcMesh, hitLocation, booleanScale](){
		FDynamicMesh3 ResultMesh;

		dyMesh_Boolean->SetWorldLocation(hitLocation);

		FTransform ftOrigin = srcMesh->GetRelativeTransform();
		FTransform ftDst = ftOrigin;
		ftDst.SetLocation(FVector(0.f));
		FTransform ftSrc = FTransform(FRotator(0.f), hitLocation - ftOrigin.GetLocation(), booleanScale);

		UE::Geometry::FMeshBoolean BooleanOp(srcMesh->GetMesh(), ftDst,
											dyMesh_Boolean->GetMesh(), ftSrc,
											&ResultMesh, UE::Geometry::FMeshBoolean::EBooleanOp::Difference);

		BooleanOp.bSimplifyAlongNewEdges = true;
		BooleanOp.bCollapseDegenerateEdgesOnCut = true;

		if (BooleanOp.Compute())
		{
			ResultMesh.CompactInPlace();
			UE::Geometry::FMergeCoincidentMeshEdges MergeEdge(&ResultMesh);
			MergeEdge.Apply();
		}

		AsyncTask(ENamedThreads::GameThread,[this, srcMesh, CreatedMesh = MoveTemp(ResultMesh)](){
			SetBoolean(srcMesh,CreatedMesh);
		});
	});
}

void UTPSGameInstanceSubsystem::SetBoolean(UDynamicMeshComponent* srcMesh, FDynamicMesh3 CreatedMesh)
{
	srcMesh->SetMesh(MoveTemp(CreatedMesh));
	srcMesh->NotifyMeshUpdated();
}

void UTPSGameInstanceSubsystem::RemoveSmallPiece(FDynamicMesh3& Mesh, float MinVolume)
{

}

void UTPSGameInstanceSubsystem::SpawnRVTEffect(FVector hitLocation, FVector effectScale, FString strName)
{
	if (set_RVTEffect.Num() > RVTEFFECT_MAX_CNT)
		set_RVTEffect.Remove(0);

	auto pActor = GetWorld()->SpawnActor<AActor>(clsRVTEffect,hitLocation,FRotator::ZeroRotator);
	if (pActor)
	{
		pActor->SetActorScale3D(effectScale);
		set_RVTEffect.Emplace(TWeakObjectPtr<AActor>(pActor));
	}
}

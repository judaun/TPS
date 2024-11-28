// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Components/DynamicMeshComponent.h"
#include "TPSGameInstanceSubsystem.generated.h"

/**
 * 
 */
enum class EBooleanOperation;
#define RVTEFFECT_MAX_CNT 20

UCLASS()
class TPSPORTFOLIO_API UTPSGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	UTPSGameInstanceSubsystem();

private:
#pragma region BooleanDynamicMesh
	void CreateBooleanSphereAsync(float Radius, int32 LatitudeSegments, int32 LongitudeSegments);
	void OnSphereCreated(FDynamicMesh3 CreatedMesh);
#pragma endregion BooleanDynamicMesh

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UDynamicMeshComponent* GetBooleanMesh();
	void MeshBoolean(UDynamicMeshComponent* srcMesh, FVector hitLocation, FVector booleanScale);
	void SetBoolean(UDynamicMeshComponent* srcMesh, FDynamicMesh3 CreatedMesh);
	void RemoveSmallPiece(FDynamicMesh3& Mesh, float MinVolume);
	
	void SpawnRVTEffect(FVector hitLocation, FVector effectScale, FString strName);
public:
	UPROPERTY()
	TObjectPtr<UDynamicMeshComponent> dyMesh_Boolean;
	UPROPERTY()
	class UMaterialInterface* BooleanMaterial;
	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> set_RVTEffect;

	UPROPERTY()
	UClass* clsRVTEffect;
};

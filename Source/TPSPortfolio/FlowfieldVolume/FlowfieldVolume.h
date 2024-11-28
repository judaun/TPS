// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/Volume.h"
#include "FlowfieldVolume.generated.h"

/**
 * 
 */
UCLASS()
class TPSPORTFOLIO_API AFlowfieldVolume : public AVolume
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(CallInEditor, Category = "Flowfield")
	void UpdateVolume();

	UFUNCTION(CallInEditor, Category = "Flowfield")
	void UpdateCellDirection();

	//~ Begin AActor Interface
	virtual void PostRegisterAllComponents() override;
	virtual void PostUnregisterAllComponents() override;
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;
	//~ End AActor Interface
#if WITH_EDITOR
	//~ Begin UObject Interface
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditUndo() override;
	//~ End UObject Interface

	static void OnPostEngineInit();
#endif // WITH_EDITOR

public:
	UPROPERTY(EditAnywhere, Category = "Flowfield")
	float CellDistance = 100.f;
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowfieldVolume.h"
#include "Engine/CollisionProfile.h"
#include "Components/BrushComponent.h"
#include "TPSGameSingleton.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowfieldVolume)

#if WITH_EDITOR
#include "ActorFactories/ActorFactory.h"
#include "Editor.h"
#endif // WITH_EDITOR

AFlowfieldVolume::AFlowfieldVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetBrushComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	GetBrushComponent()->Mobility = EComponentMobility::Static;

	BrushColor = FColor(200,150,150,255);
	bColored = true;
#if WITH_EDITORONLY_DATA
	bIsSpatiallyLoaded = false;
#endif
}

void AFlowfieldVolume::UpdateVolume()
{
	UTPSGameSingleton& GameSingleton = UTPSGameSingleton::Get();
	GameSingleton.Update_FFPathfinder(this, CellDistance);
}

void AFlowfieldVolume::UpdateCellDirection()
{
	UTPSGameSingleton& GameSingleton = UTPSGameSingleton::Get();
	
	FTransform trans;
	auto pActor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass());
	if(nullptr != pActor)
		trans = pActor->GetTransform();

	UE_LOG(LogTemp, Warning, TEXT("UpdateCellDirection"));
	UE_LOG(LogTemp, Warning, TEXT("trans : %f, %f"), trans.GetTranslation().X, trans.GetTranslation().Y);


	GameSingleton.Update_FFPathDirection(trans);
	GameSingleton.DebugView_FFPathDirection(GetWorld());


}

void AFlowfieldVolume::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();

	UTPSGameSingleton& GameSingleton = UTPSGameSingleton::Get();
	GameSingleton.Set_FFPathfinder();
	//GameSingleton.Update_FFPathfinder(this, CellDistance);
}

void AFlowfieldVolume::PostUnregisterAllComponents()
{
	Super::PostUnregisterAllComponents();
}

void AFlowfieldVolume::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//UTPSGameSingleton& GameSingleton = UTPSGameSingleton::Get();
	//GameSingleton.Update_FFPathfinder(this, CellDistance);
}

void AFlowfieldVolume::BeginPlay()
{
	Super::BeginPlay();
	UTPSGameSingleton& GameSingleton = UTPSGameSingleton::Get();
	GameSingleton.Update_FFPathfinder(this, CellDistance);
}

void AFlowfieldVolume::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UTPSGameSingleton& GameSingleton = UTPSGameSingleton::Get();
	GameSingleton.Update_FFPathfinder(this, CellDistance);
}

void AFlowfieldVolume::PostEditUndo()
{
	Super::PostEditUndo();
}

void AFlowfieldVolume::OnPostEngineInit()
{

}

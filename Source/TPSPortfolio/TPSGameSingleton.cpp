#include "TPSGameSingleton.h"
#include "FlowfieldPathfinder.h"
#include "FlowfieldVolume.h"
#include "FlowCell.h"

UTPSGameSingleton::UTPSGameSingleton()
{

}

UTPSGameSingleton& UTPSGameSingleton::Get()
{
	if (IsValid(GEngine->GameSingleton))
	{
		UTPSGameSingleton* GameSingleton = CastChecked<UTPSGameSingleton>(GEngine->GameSingleton);
		if(GameSingleton)
			return *GameSingleton;
	}
	
	return *NewObject<UTPSGameSingleton>();
}

UFlowfieldPathfinder* UTPSGameSingleton::Get_FFPathfinder()
{
	if(IsValid(Ff_Pathfinder)) return Ff_Pathfinder;

	return nullptr;
}

void UTPSGameSingleton::Set_FFPathfinder()
{
	if (IsValid(Ff_Pathfinder)) return;

	Ff_Pathfinder = NewObject<UFlowfieldPathfinder>(this, UFlowfieldPathfinder::StaticClass());
}

void UTPSGameSingleton::Update_FFPathfinder(AFlowfieldVolume* volume, float distance)
{
	if (!IsValid(Ff_Pathfinder)) return;
	UE_LOG(LogTemp, Warning, TEXT("Updated_FFPathfinder!"));
	Ff_Pathfinder->UpdateFlowfield(volume, volume->GetWorld(), distance);
}

void UTPSGameSingleton::Update_FFPathDirection(FTransform& target)
{
	if (!IsValid(Ff_Pathfinder)) return;
	Ff_Pathfinder->SetFlowDirections(target);
}

void UTPSGameSingleton::DebugView_FFPathDirection(UWorld* InWorld)
{
	if (!IsValid(Ff_Pathfinder)) return;
	Ff_Pathfinder->ShowDebugDirection(InWorld);
}

FVector UTPSGameSingleton::GetDirection(FTransform& transform)
{
	if (!IsValid(Ff_Pathfinder)) return FVector(0.f);
	return Ff_Pathfinder->GetFlowDirection(transform);
}

UFlowCell* UTPSGameSingleton::GetCell(FTransform& transform)
{
	if (!IsValid(Ff_Pathfinder)) return nullptr;
	return Ff_Pathfinder->FindNearCell(transform);
}

UFlowCell* UTPSGameSingleton::GetAccessCell(FTransform& transform)
{
	if (!IsValid(Ff_Pathfinder)) return nullptr;
	return Ff_Pathfinder->FindNearAccessCell(transform);
}
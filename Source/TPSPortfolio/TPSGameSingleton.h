#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TPSGameSingleton.generated.h"

class UFlowfieldPathfinder;
class AFlowfieldVolume;
class UFlowCell;

UCLASS()
class UTPSGameSingleton : public UObject
{
    GENERATED_BODY()

public:
    UTPSGameSingleton();

public:
    static UTPSGameSingleton& Get();

    /* Flowfield Pathfinder */
#pragma region Flowfield_Pathfinder
    UFlowfieldPathfinder* Get_FFPathfinder();
    void Set_FFPathfinder();
    void Update_FFPathfinder(AFlowfieldVolume* volume, float distance);
    void Update_FFPathDirection(FTransform& target);
    void DebugView_FFPathDirection(UWorld* InWorld);

    FVector GetDirection(FTransform& transform);
	UFlowCell* GetCell(FTransform& transform);
	UFlowCell* GetAccessCell(FTransform& transform);
#pragma endregion

   
protected:
private:
    UPROPERTY()
    TObjectPtr<UFlowfieldPathfinder> Ff_Pathfinder;
};


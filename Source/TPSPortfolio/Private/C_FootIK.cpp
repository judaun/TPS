// Fill out your copyright notice in the Description page of Project Settings.


#include "C_FootIK.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "CalculationFunction.h"

// Sets default values for this component's properties
UC_FootIK::UC_FootIK()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	rotFoot_L = FRotator::ZeroRotator;
	rotFoot_R = FRotator::ZeroRotator;
	fHipOffset = 0.f;
	fHalfCapsuleHeight = 0.f;
	// ...
}


// Called when the game starts
void UC_FootIK::BeginPlay()
{
	Super::BeginPlay();

	// ...

	auto pOwner = GetOwner();
	if (nullptr == pOwner) return;
	auto pCharacter = Cast<ACharacter>(pOwner);
	if (nullptr == pCharacter) return;
	auto pCapsule = pCharacter->GetCapsuleComponent();
	if (nullptr == pCapsule) return;
	fHalfCapsuleHeight = pCapsule->GetScaledCapsuleHalfHeight();
}


void UC_FootIK::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void UC_FootIK::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	FIKHitData stHitData_L = FootIKTrace(50.f, FName("L_Foot"));
	FIKHitData stHitData_R = FootIKTrace(50.f, FName("R_Foot"));

	UpdateRotator(stHitData_L, &rotFoot_L, DeltaTime);
	UpdateRotator(stHitData_R, &rotFoot_R, DeltaTime);

	float fHipsOffset = UKismetMathLibrary::Min(stHitData_L.fFootOffset, stHitData_R.fFootOffset);
	if (fHipsOffset < 0.0f == false) fHipsOffset = 0.0f;
	UpdateFootOffset(DeltaTime, fHipsOffset, &fHipOffset, 13.f);
	UpdateCapsuleHeight(DeltaTime, fHipsOffset);

	UpdateFootOffset(DeltaTime, stHitData_L.fFootOffset- fHipsOffset, &fFootOffset_L, 13.f);
	UpdateFootOffset(DeltaTime, stHitData_R.fFootOffset - fHipsOffset, &fFootOffset_R, 13.f);
}

FIKHitData UC_FootIK::FootIKTrace(float tracedistance, FName socketname)
{
	FIKHitData stHitData;

	auto pOwner = GetOwner();
	if (nullptr == pOwner) return stHitData;
	auto pCharacter = Cast<ACharacter>(pOwner);
	if (nullptr == pCharacter) return stHitData;
	auto pMesh = pCharacter->GetMesh();
	if (nullptr == pMesh) return stHitData;

	float fActorLocationZ = pOwner->GetActorLocation().Z;

	FVector vSocketLocation = pMesh->GetSocketLocation(socketname);
	FVector vStart = FVector(vSocketLocation.X, vSocketLocation.Y, fActorLocationZ);
	FVector vEnd = FVector(vSocketLocation.X, vSocketLocation.Y, (fActorLocationZ - fHalfCapsuleHeight) - tracedistance);

	//LineTrace
	FHitResult stHitResult;
	TArray<AActor*> vecIgnore;
	vecIgnore.Add(pOwner);

	bool bDebug = true;
	EDrawDebugTrace::Type eDebug = EDrawDebugTrace::None;
	if (bDebug == true) 
		eDebug = EDrawDebugTrace::ForOneFrame;

	bool isTrace = UKismetSystemLibrary::LineTraceSingle(GetWorld(), vStart, vEnd,
		UEngineTypes::ConvertToTraceType(ECC_Visibility), true, vecIgnore, eDebug, stHitResult, true);

	stHitData.vHitNormal = stHitResult.ImpactNormal;
	if (stHitResult.IsValidBlockingHit())
	{
		float fHitlen = (stHitResult.ImpactPoint - stHitResult.TraceEnd).Size();
		stHitData.fFootOffset = (fHitlen - tracedistance);
	}
	else
		stHitData.fFootOffset = 0.f;

	return stHitData;
}

void UC_FootIK::UpdateRotator(FIKHitData hitdata, FRotator* rotdata, float deltaseconds)
{

	float fAtan2_1 = UKismetMathLibrary::DegAtan2(hitdata.vHitNormal.Y, hitdata.vHitNormal.Z);
	float fAtan2_2 = UKismetMathLibrary::DegAtan2(hitdata.vHitNormal.X, hitdata.vHitNormal.Z);
	fAtan2_2 *= -1.0f;
	FRotator pResult = FRotator(fAtan2_2, 0.0f, fAtan2_1);

	*rotdata = FMath::RInterpTo(*rotdata, pResult, deltaseconds, 13.f);
}

void UC_FootIK::UpdateCapsuleHeight(float Deltatime, float hipoffset)
{
	auto pOwner = GetOwner();
	if (nullptr == pOwner) return;
	auto pCharacter = Cast<ACharacter>(pOwner);
	if (nullptr == pCharacter) return;
	auto pCapsule = pCharacter->GetCapsuleComponent();
	if (nullptr == pCapsule) return;

	
	float fOffsetsize = abs(hipoffset)*0.5f;
	float fCapsuleHalf = fHalfCapsuleHeight - fOffsetsize;

	float fScCapsuleHalf = pCapsule->GetScaledCapsuleHalfHeight();
	if (abs(fScCapsuleHalf - fScCapsuleHalf) < 0.1f)
		return;

	float fInterpValue = FMath::FInterpTo(fScCapsuleHalf, fCapsuleHalf, Deltatime, 13.f);

	pCapsule->SetCapsuleHalfHeight(fInterpValue, true);
}

void UC_FootIK::UpdateFootOffset(float Deltatime, float targetvalue, float* effvalue, float interpspeed)
{
	float fInterpValue = UKismetMathLibrary::FInterpTo(*effvalue, targetvalue, Deltatime, interpspeed);
	*effvalue = fInterpValue;
}


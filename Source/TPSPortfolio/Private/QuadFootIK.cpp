// Fill out your copyright notice in the Description page of Project Settings.


#include "QuadFootIK.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Curves/CurveVector.h"

// Sets default values for this component's properties
UQuadFootIK::UQuadFootIK()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


	TA_IKData.Emplace(FQuadIKData{ FName("SpiderBot_Foot_Front_Left"), FName("SocketLF") });
	TA_IKData.Emplace(FQuadIKData{ FName("SpiderBot_Foot_Front_Right"), FName("SocketRF") });
	TA_IKData.Emplace(FQuadIKData{ FName("SpiderBot_Foot_Rear_Left"), FName("SocketLB") });
	TA_IKData.Emplace(FQuadIKData{ FName("SpiderBot_Foot_Rear_Right"), FName("SocketRB") });

	ConstructorHelpers::FObjectFinder<UCurveVector> FOBJ_Curve(TEXT("/Script/Engine.CurveVector'/Game/ThirdPerson/Curves/VectorSpiderLegCurve.VectorSpiderLegCurve'"));
	if (FOBJ_Curve.Succeeded())
	{
		MoveCurve = FOBJ_Curve.Object;
	}
}


// Called when the game starts
void UQuadFootIK::BeginPlay()
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

	UpdateFootLocation();
	vDstFrameLocation = GetOwner()->GetActorLocation();
}


// Called every frame
void UQuadFootIK::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
	FootIKTrace(50.f, &TA_IKData[0]);
	FootIKTrace(50.f, &TA_IKData[1]);
	FootIKTrace(50.f, &TA_IKData[2]);
	FootIKTrace(50.f, &TA_IKData[3]);


	float fHipsOffset = (TA_IKData[0].fFootOffset + TA_IKData[1].fFootOffset + TA_IKData[2].fFootOffset + TA_IKData[3].fFootOffset) / 4.f;

	if (fHipsOffset < 0.0f == false) fHipsOffset = 0.0f;
	UpdateFootOffset(DeltaTime, fHipsOffset, &fHipOffset, 13.f);
	UpdateCapsuleHeight(DeltaTime, fHipsOffset);

	vSrcFrameLocation = GetOwner()->GetActorLocation() - vDstFrameLocation;
	vSrcFrameLocation.Z = 0;
	vDstFrameLocation = GetOwner()->GetActorLocation();

	for (auto& elem : TA_IKData)
	{
		if (elem.fMoving > 0.f)
		{
			elem.vDstLocation += vSrcFrameLocation;
			elem.fMoving -= DeltaTime;
		}

	}

	for (auto& elem : TA_IKData)
		UpdateVectorOffset(DeltaTime, elem.vDstLocation, &elem.vLocation, &elem.fCurveData, 20.f);

	for (auto& elem : TA_IKData)
	{
		elem.vLocation.Z = elem.fWorldOffset + elem.fCurveData;
	}

	UpdateBodyRoll(DeltaTime);
}

void UQuadFootIK::SetLegData(bool left, bool front, FName bonename, FName socketname)
{
	if(TA_IKData.Num() < 4) return;
	
	front ?
		left ? SetLegData(&TA_IKData[0], bonename, socketname) 
				: SetLegData(&TA_IKData[1], bonename, socketname) :
		left ? SetLegData(&TA_IKData[2], bonename, socketname) 
				: SetLegData(&TA_IKData[3], bonename, socketname);
}

void UQuadFootIK::SetLegData(FQuadIKData* ikdata, FName bonename, FName socketname)
{
	ikdata->BoneName = bonename;
	ikdata->SocketName = socketname;
}

void UQuadFootIK::FootIKTrace(float tracedistance, FQuadIKData* ikdata)
{
	auto pOwner = GetOwner();
	if (nullptr == pOwner) return;
	auto pCharacter = Cast<ACharacter>(pOwner);
	if (nullptr == pCharacter) return;
	auto pMesh = pCharacter->GetMesh();
	if (nullptr == pMesh) return;

	float fActorLocationZ = pOwner->GetActorLocation().Z;

	FVector vBoneLocation = pMesh->GetSocketLocation(ikdata->BoneName);
	FVector vStart = FVector(vBoneLocation.X, vBoneLocation.Y, fActorLocationZ);
	FVector vEnd = FVector(vBoneLocation.X, vBoneLocation.Y, (fActorLocationZ - fHalfCapsuleHeight) - tracedistance);

	//LineTrace
	FHitResult stHitResult;
	TArray<AActor*> vecIgnore;
	vecIgnore.Add(pOwner);

	bool isTrace = UKismetSystemLibrary::LineTraceSingle(GetWorld(), vStart, vEnd,
		UEngineTypes::ConvertToTraceType(ECC_Visibility), true, vecIgnore, EDrawDebugTrace::None, stHitResult, true);

	if (stHitResult.IsValidBlockingHit())
	{
		float fHitlen = (stHitResult.ImpactPoint - stHitResult.TraceEnd).Size();
		ikdata->fFootOffset = (fHitlen - tracedistance);
		ikdata->fWorldOffset = stHitResult.ImpactPoint.Z + 15.f;
	}
	else
	{
		ikdata->fFootOffset = 0.f;
		ikdata->fWorldOffset = vBoneLocation.Z - 15.f;
	}

	FVector vTargetLocation = pMesh->GetSocketLocation(ikdata->SocketName);
	FVector vDst = FVector(ikdata->vLocation.X, ikdata->vLocation.Y, 0.f);
	FVector vSrc = FVector(vTargetLocation.X, vTargetLocation.Y, 0.f);

	if ((vDst - vSrc).Length() > MAXDISTANCE)
	{
		ikdata->vDstLocation = vTargetLocation + (vSrc - vDst).GetSafeNormal() * 30.f;
		ikdata->fMoving = 0.1f;
	}

	//DrawDebugBox(GetWorld(), ikdata->vDstLocation, FVector(3.f), FColor::Cyan);
	//FHitResult stHitResult2;
	//DrawDebugBox(GetWorld(), vTargetLocation, FVector(5.f), FColor::Blue);
	//UKismetSystemLibrary::LineTraceSingle(GetWorld(), ikdata->vLocation, vTargetLocation,
	//	UEngineTypes::ConvertToTraceType(ECC_Visibility), true, vecIgnore, EDrawDebugTrace::ForOneFrame, stHitResult2, true, FLinearColor::Blue);
}

void UQuadFootIK::UpdateCapsuleHeight(float Deltatime, float hipoffset)
{
	auto pOwner = GetOwner();
	if (nullptr == pOwner) return;
	auto pCharacter = Cast<ACharacter>(pOwner);
	if (nullptr == pCharacter) return;
	auto pCapsule = pCharacter->GetCapsuleComponent();
	if (nullptr == pCapsule) return;


	float fOffsetsize = abs(hipoffset) * 0.5f;
	float fCapsuleHalf = fHalfCapsuleHeight - fOffsetsize;

	float fScCapsuleHalf = pCapsule->GetScaledCapsuleHalfHeight();
	if (abs(fScCapsuleHalf - fScCapsuleHalf) < 0.1f)
		return;

	float fInterpValue = FMath::FInterpTo(fScCapsuleHalf, fCapsuleHalf, Deltatime, 13.f);

	pCapsule->SetCapsuleHalfHeight(fInterpValue, true);
}

void UQuadFootIK::UpdateFootOffset(float Deltatime, float targetvalue, float* effvalue, float interpspeed)
{
	float fInterpValue = UKismetMathLibrary::FInterpTo(*effvalue, targetvalue, Deltatime, interpspeed);
	*effvalue = fInterpValue;
}

void UQuadFootIK::UpdateVectorOffset(float Deltatime, FVector targetvalue, FVector* effvalue, float* curvevalue, float interpspeed)
{
	FVector vInterpValue = UKismetMathLibrary::VInterpTo(*effvalue, targetvalue, Deltatime, interpspeed);
	*effvalue = vInterpValue;

	if (nullptr == MoveCurve) return;
	float fTime = (FVector(targetvalue.X, targetvalue.Y, 0.f) - FVector(effvalue->X, effvalue->Y, 0.f)).Length() / MAXDISTANCE;
	*curvevalue = MoveCurve->GetVectorValue(fTime).Z * 40.f;
}

void UQuadFootIK::UpdateFootLocation()
{
	auto pOwner = GetOwner();
	if (nullptr == pOwner) return;
	auto pCharacter = Cast<ACharacter>(pOwner);
	if (nullptr == pCharacter) return;
	auto pMesh = pCharacter->GetMesh();
	if (nullptr == pMesh) return;

	TA_IKData[0].vLocation = pMesh->GetSocketLocation(TA_IKData[0].BoneName);
	TA_IKData[1].vLocation = pMesh->GetSocketLocation(TA_IKData[1].BoneName);
	TA_IKData[2].vLocation = pMesh->GetSocketLocation(TA_IKData[2].BoneName);
	TA_IKData[3].vLocation = pMesh->GetSocketLocation(TA_IKData[3].BoneName);
}


void UQuadFootIK::UpdateBodyRoll(float DeltaTime)
{
	float fL = TA_IKData[0].vLocation.Z + TA_IKData[2].vLocation.Z;
	float fR = TA_IKData[1].vLocation.Z + TA_IKData[3].vLocation.Z;

	rotBody.Pitch = UKismetMathLibrary::FInterpTo(rotBody.Pitch, (fL - fR) / 3.f, DeltaTime, 3.f);
}

FVector UQuadFootIK::GetFootVectorOffset(bool left, bool front)
{
	return front ?
		left ? TA_IKData[0].vLocation : TA_IKData[1].vLocation :
		left ? TA_IKData[2].vLocation : TA_IKData[3].vLocation;
}


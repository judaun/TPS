// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


static float CalculateDegree(const FVector SourceVector, const FVector DestVecter)
{
	float fDegree = FMath::RadiansToDegrees(FMath::Acos(SourceVector.Dot(DestVecter)));
	return SourceVector.Cross(DestVecter).Z < 0 ? -fDegree : fDegree;
}

static void CalculateDegree(const FVector SourceVector, const FVector DestVecter, float& Cross, float& Degree)
{
	Cross = SourceVector.Cross(DestVecter).Z;
	Degree = FMath::RadiansToDegrees(FMath::Acos(SourceVector.Dot(DestVecter)));
}

static float InterpConstantToVector(const FVector SourceVector, const FVector DestVecter, float DeltaSeconds, float TurnSpeed)
{
	return FMath::FInterpConstantTo(0, CalculateDegree(SourceVector, DestVecter), DeltaSeconds, TurnSpeed);
}

static float InterpConstantToVector(const FVector SourceVector, const FVector DestVecter, float& Cross, float& Degree, float DeltaSeconds, float TurnSpeed)
{
	CalculateDegree(SourceVector, DestVecter, Cross, Degree);
	return FMath::FInterpConstantTo(0, Cross < 0.f ? -Degree: Degree, DeltaSeconds, TurnSpeed);
}

static float InterpToVector(const FVector SourceVector, const FVector DestVecte, float DeltaSeconds, float TurnSpeed)
{
	return FMath::FInterpTo(0, CalculateDegree(SourceVector, DestVecte), DeltaSeconds, TurnSpeed);
}
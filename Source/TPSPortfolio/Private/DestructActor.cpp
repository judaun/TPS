// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructActor.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Field/FieldSystemComponent.h"

// Sets default values
ADestructActor::ADestructActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	GeometryCollectionComponent = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("Geometry"));
	RootComponent = GeometryCollectionComponent;
	
	FieldSystemComponent = CreateDefaultSubobject<UFieldSystemComponent>(TEXT("FieldSystemComponent"));
	FieldSystemComponent->SetupAttachment(RootComponent);
	
	GeometryCollectionComponent->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void ADestructActor::BeginPlay()
{
	Super::BeginPlay();
	GeometryCollectionComponent->SetEnableDamageFromCollision(false);
}

// Called every frame
void ADestructActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ADestructActor::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float fDmg = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (!IsValid(GeometryCollectionComponent)) return fDmg;

	//포인트 데미지 타입 검사
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		
		FieldSystemComponent->ApplyStrainField(true, PointDamageEvent->HitInfo.ImpactPoint, 100.f, 5000000.f, 0);
		GeometryCollectionComponent->AddRadialImpulse(PointDamageEvent->HitInfo.ImpactPoint, 50.f,100.f,ERadialImpulseFalloff::RIF_Constant,true);
	}
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		const FRadialDamageEvent* RadialDamageEvent = static_cast<const FRadialDamageEvent*>(&DamageEvent);
	
		FVector vSrc = RadialDamageEvent->Origin;

		FieldSystemComponent->ApplyStrainField(true, vSrc, RadialDamageEvent->Params.GetMaxRadius(), 5000000.f, 0);
		GeometryCollectionComponent->AddRadialImpulse(vSrc, RadialDamageEvent->Params.GetMaxRadius(), 1000.f, ERadialImpulseFalloff::RIF_Constant, true);

		UE_LOG(LogTemp,Log,TEXT("RadialDamaged"));
	}

	return fDmg;
}


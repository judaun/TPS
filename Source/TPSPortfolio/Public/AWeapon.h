// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment.h"
#include "GameFramework/Actor.h"
#include "Components/TimeLineComponent.h"
#include "AWeapon.generated.h"

enum class EWeaponType : uint8;
class ATPSPortfolioCharacter;
class AMagazine;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class TPSPORTFOLIO_API AWeapon : public AActor , public Equipment
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();
	AWeapon(FEquipmentTable* equipdata);
	~AWeapon();

private:
	void InitializeMesh(FString weaponaddress);
	FString GetWeaponTypeName(EWeaponType weapontype);
	void InitMagazineMesh(FString magazineaddress);
	void InitTimeLine();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	void SetPlayer(ATPSPortfolioCharacter* character);
	void DeferredInitialize(FEquipmentTable* equipdata);
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void AttackTrace();
	void Reload();
	void ReloadStart();
	void AttackStart();
	void AttackStop();
	void SetAimRate(float aimrate) { fAimRate = aimrate; }
	int32 GetCurrentBullet() { return iCurrentCapacity; }
	int32 GetMaxBullet() { return FEquipData.iBaseCapacity; }
	int32 GetMagazine() { return iCurrentMagazine; }

	FVector GetAimrateRecoilPosition();
	bool IsFullCapacity();
	bool IsPosibleReload();

	UFUNCTION()
	void OnCameraRecoilProgress(FVector CameraRecoil);

	UFUNCTION()
	void OnRecoilTimelineFinish();

private:
	void SetSpawnDecal(FVector Location, FRotator Rotator);

private:
	UPROPERTY()
	USkeletalMeshComponent* pMesh;

	FCollisionQueryParams collisionParams;
	FTimerHandle Firetimehandle;
	UPROPERTY()
	TWeakObjectPtr<ATPSPortfolioCharacter> pCharacter;
	int32 iCurrentCapacity;
	int32 iCurrentMagazine;

	float fPitchRecoil;
	float fYawRecoil;
	float fAimRate;

	UPROPERTY()
	AMagazine* pMagazine;

	UPROPERTY()
	TSubclassOf<UCameraShakeBase> CS_Attack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Timeline", Meta = (AllowPrivateAccess = "true"))
	UCurveVector* CameraRecoilCurve;

	UPROPERTY(EditAnywhere, Category = "Firing")
	UNiagaraSystem* FireEffectMuzzle;
	UPROPERTY(EditAnywhere, Category = "Firing")
	UNiagaraComponent* pNiagaraCom;

	UPROPERTY(EditAnywhere, Category = "Hit")
	UNiagaraSystem* HitEffect;

	UPROPERTY()
	UMaterial* pDecalMaterial;

	UPROPERTY()
	FTimeline RecoilTimeline;
	float RecoilRecoveryTime;
	float CurrentRecoilRecoveryTime;
};

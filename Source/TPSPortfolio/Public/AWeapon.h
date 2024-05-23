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

//function///////////////////////////////////////////////////////
private:
	void InitializeMesh(FString weaponaddress);
	FString GetWeaponTypeName(EWeaponType weapontype);
	void InitMagazineMesh();
	void InitTimeLine();
	void SetSpawnDecal(FVector Location, FRotator Rotator);
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
	/* 플레이어 넘겨받아 WeakPtr로 보관 */
	void SetPlayer(ATPSPortfolioCharacter* character);
	/* DeferredSpawn으로 beginplay 전 데이터 세팅 */
	void DeferredInitialize(FEquipmentTable* equipdata);
	/* 공격용 트레이스 생성 */
	void AttackTrace();
	/* 재장전 완료 */
	void Reload();
	/* 재장전 시작 */
	void ReloadStart();
	void AttackStart();
	void AttackStop();
	void SetAimRate(float aimrate) { fAimRate = aimrate; }
	void SetHide(bool hide);
	int32 GetCurrentBullet() { return iCurrentCapacity; }
	int32 GetMaxBullet() { return FEquipData.iBaseCapacity; }
	int32 GetMagazine() { return iCurrentMagazine; }

	EWeaponType GetWeaponType() { return FEquipData.WeaponType; }

	void GetAimrateRecoilPosition(float* yawangle, float* pitchangle);
	bool IsFullCapacity();
	bool IsPosibleReload();

	UFUNCTION()
	void OnCameraRecoilProgress(FVector CameraRecoil);

	UFUNCTION()
	void OnRecoilTimelineFinish();

//value///////////////////////////////////////////////////////
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
	UNiagaraComponent* pNiagaraCom;

	UPROPERTY()
	FTimeline RecoilTimeline;
	float RecoilRecoveryTime;
	float CurrentRecoilRecoveryTime;

	float fFireMenual;

	UPROPERTY()
	UAnimSequence* pShotAnim;
};

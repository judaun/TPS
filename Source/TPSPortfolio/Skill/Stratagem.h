// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerSkill.h"
#include "Components/TimeLineComponent.h"
#include "Stratagem.generated.h"

enum class ESkillEffectType : uint8
{
	SKILL_READY, SKILL_DEPLOY, SKILL_COUNTDOWN, SKILL_EFFECT, SKILL_END,
};

class ATPSPortfolioCharacter;
class USplineMeshComponent;
class UNiagaraComponent;
class UWidgetComponent;
struct FPredictProjectilePathPointData;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FDele_Stratagem_Time, int32, int32, float);

UCLASS()
class TPSPORTFOLIO_API AStratagem : public AActor, public UPlayerSkill
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStratagem();

private:
	void InitDelegate();

	void Deployment();
	void Drop();
	void EffectEnd();
	void Effect(float Deltatime);
	void ChangeState(ESkillEffectType state);
	void UpdateSpline(TArray<FPredictProjectilePathPointData>& PathData);
	void ClearSpline();
	void ResetStratagem();
	void TimeBroadcast();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	void NotifyActorBeginOverlap(AActor* OtherActor) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void DeferredInitialize();
	/*스트라타젬에 데이터 주입*/
	void SetData(FSkillTable* data);
	void SetData(FSkillTable data);
	void SetHide(bool ishide);
	void SetCharacterOwner(ATPSPortfolioCharacter* powner, int32 owerskillidx);
	int32 GetSkillCommand() {return fSkillTable.Command;}

	bool IsReady();

#pragma region Stratagem
	void ArcTrace();
	void ArcAttack();
#pragma endregion
private:
	UPROPERTY()
	TWeakObjectPtr<ATPSPortfolioCharacter> pOwnerPlayer; 
	
	UPROPERTY()
	UStaticMeshComponent* pMesh;

	UPROPERTY(EditAnywhere, Category = "DeployEffect")
	UNiagaraComponent* pNiagaraCom;

	UPROPERTY(EditAnywhere, Category = "UIComponent")
	UWidgetComponent* pUIComponent;

#pragma region spline
	UPROPERTY()
		TArray<USplineMeshComponent*> ta_Spline_Mesh;

	UPROPERTY()
		class USplineComponent* Spline_Path;

	UPROPERTY()
		class UStaticMesh* SplineMesh;

	UPROPERTY()
		class UMaterialInterface* SplineMaterial;
#pragma endregion spline

	FTimerHandle StratagemTimeHandle;

	float fEffectTime;
	float fDelayTime;
	float fCountDown;
	float fCoolDown;

	int32 iPlayerSkillIdx;

	ESkillEffectType eGemeffecttype;

public:
	FDele_Stratagem_Time func_Stratagem_Time;
};

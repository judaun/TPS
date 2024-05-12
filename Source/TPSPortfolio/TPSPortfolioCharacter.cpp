// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSPortfolioCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "TPSPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "IdleCharacterState.h"
#include "RunCharacterState.h"
#include "BrakeCharacterState.h"
#include "SprintCharacterState.h"
#include "AimCharacterState.h"
#include "EvadeCharacterState.h"
#include "CrossHair.h"
#include "CharacterHUD.h"
#include "AWeapon.h"
#include "TPSAnimInstance.h"
#include "C_FootIK.h"

//////////////////////////////////////////////////////////////////////////
// ATPSPortfolioCharacter

ATPSPortfolioCharacter::ATPSPortfolioCharacter()
{
	InitializeDefaultComponent();
	InitializeInputContext();
	InitializeMeshComponent();

	initialize();
}

ATPSPortfolioCharacter::~ATPSPortfolioCharacter()
{
	vecState.clear();
	func_Player_Bullet.Clear();
	func_Player_Aimrate.Clear();
	func_Player_Magazine.Unbind();
	WeaponSlot.Reset();

	if (pCurWeapon != nullptr && IsValid(pCurWeapon))
	{
		pCurWeapon->Reset();
	}
}

void ATPSPortfolioCharacter::initialize()
{
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 0.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 750.f;

	vChangeDirection = GetActorForwardVector();
	vLerpDirection = vChangeDirection;
	vControlVectorX = FVector(1.f, 0.f, 0.f);
	vControlVectorY = FVector(1.f, 0.f, 0.f);
	vAimPosition = FVector(1.f, 0.f, 0.f);

	fCrossAngle = 0.f;
	fYCrossAngle = 0.f;
	fTurnSpeed = 360.f;
	fDefaultWalkSpeed = 150.f;
	fRunSpeed = 450.f;
	fSprintSpeed = 800.f;
	fBrakeTimer = 0.f;
	fFrontAcos = 1.f;
	iWeaponIndex = 0;
	bIsRun = true;
	bIsSprint = false;
	bIsMoving = false;
	bIsBraking = false;
	bIsAiming = false;
	bIsAimTurn = false;
	bIsEquiping = false;
	bIsEvade = false;
	bIsCrawl = false;
	bIsCrawltoIdle = false;

	//auto stState = new IdleState();
	//auto stState = new IdleCharacterState(this);
	vecState.emplace_back(make_unique<IdleCharacterState>(this));
	vecState.emplace_back(make_unique<RunCharacterState>(this));
	vecState.emplace_back(make_unique<BrakeCharacterState>(this));
	vecState.emplace_back(make_unique<SprintCharacterState>(this));
	vecState.emplace_back(make_unique<AimCharacterState>(this));
	vecState.emplace_back(make_unique<EvadeCharacterState>(this));

	stCharacterState = vecState[0].get();
}

void ATPSPortfolioCharacter::InitializeInputContext()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> FObj_InputContext(TEXT("/Game/ThirdPerson/Input/IMC_Default.IMC_Default"));
	if (FObj_InputContext.Succeeded())
	{
		UE_LOG(LogTemp, Log, TEXT("InputTest"));
		DefaultMappingContext = FObj_InputContext.Object;
	}

	IAFactory(TEXT("/Game/ThirdPerson/Input/Actions/IA_Jump.IA_Jump"), &JumpAction);
	IAFactory(TEXT("/Game/ThirdPerson/Input/Actions/IA_Aim.IA_Aim"), &AimAction);
	IAFactory(TEXT("/Game/ThirdPerson/Input/Actions/IA_Look.IA_Look"), &LookAction);
	IAFactory(TEXT("/Game/ThirdPerson/Input/Actions/IA_Move.IA_Move"), &MoveAction);
	IAFactory(TEXT("/Game/ThirdPerson/Input/Actions/IA_Run.IA_Run"), &RunAction);
	IAFactory(TEXT("/Game/ThirdPerson/Input/Actions/IA_Sprint.IA_Sprint"), &SprintAction);
	IAFactory(TEXT("/Game/ThirdPerson/Input/Actions/IA_Attack.IA_Attack"), &AttackAction);
	IAFactory(TEXT("/Game/ThirdPerson/Input/Actions/IA_Reload.IA_Reload"), &ReloadAction);
	IAFactory(TEXT("/Game/ThirdPerson/Input/Actions/IA_Primary.IA_Primary"), &Weapon1Action);
	IAFactory(TEXT("/Game/ThirdPerson/Input/Actions/IA_Secondary.IA_Secondary"), &Weapon2Action);
}

void ATPSPortfolioCharacter::InitializeMeshComponent()
{
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, 270.f, 0.f));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> FObj_Skmesh(TEXT("/Game/Characters/PlayerCharacters/Vepley/Mesh/Vepley.Vepley"));
	if (FObj_Skmesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(FObj_Skmesh.Object);
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	}

	ConstructorHelpers::FClassFinder<UAnimInstance> FObj_Anim(TEXT("Blueprint'/Game/Characters/PlayerCharacters/Vepley/ABP_Vepley.ABP_Vepley_C'"));
	if (FObj_Anim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(FObj_Anim.Class);
	}

	static ConstructorHelpers::FObjectFinder<UPhysicsAsset> FObj_Physics(TEXT("/Game/Characters/PlayerCharacters/Vepley/Mesh/Vepleysize10_Physics.Vepleysize10_Physics"));
	if (FObj_Physics.Succeeded())
	{
		GetMesh()->SetPhysicsAsset(FObj_Physics.Object);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionProfileName(TEXT("TPSCharacter"));
	}
}

void ATPSPortfolioCharacter::InitializeDefaultComponent()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 90.0f);
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 350.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SocketOffset = FVector(0.f, 45.f, 70.f);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	pInventory = CreateDefaultSubobject<UInventory>(TEXT("Inventory"));
	FootIK = CreateDefaultSubobject<UC_FootIK>(TEXT("FootIK"));
}

void ATPSPortfolioCharacter::IAFactory(FString address, UInputAction** uiaction)
{
	ConstructorHelpers::FObjectFinder<UInputAction>IA_Action
	(*address);
	if (IA_Action.Succeeded())
		*uiaction = IA_Action.Object;
}

void ATPSPortfolioCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}


	FName WeaponSocket(TEXT("r_hand_rifle"));
	WeaponSlot.Emplace(pInventory->LoadWeapon(0));
	WeaponSlot.Emplace(pInventory->LoadWeapon(1));
	WeaponSlot.Emplace(pInventory->LoadWeapon(2));
	
	for (auto elem : WeaponSlot)
	{
		elem->SetPlayer(this);
		elem->SetHide(true);
	}
		

	pCurWeapon = WeaponSlot[0];
	pCurWeapon->SetHide(false);
	pCurWeapon->SetPlayer(this);
	//pCurWeapon = GetWorld()->SpawnActor<AWeapon>(FVector::ZeroVector, FRotator::ZeroRotator);

	if (GetMesh()->DoesSocketExist(WeaponSocket) && pCurWeapon != nullptr)
	{
		pCurWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
	}

	if (ATPSPlayerController* TPSController = Cast<ATPSPlayerController>(Controller))
	{
		if (IsValid(TPSController->CrossHairHUDWidget))
		{
			Cast<UCrossHair>(TPSController->CrossHairHUDWidget)->BindUserAimRate(this);
			
		}	
		if (IsValid(TPSController->CharacterHUDWidget))
		{
			Cast<UCharacterHUD>(TPSController->CharacterHUDWidget)->BindUserData(this);
		}
	}

	if (func_Player_Aimrate.IsBound())
		func_Player_Aimrate.Broadcast(0.f);

	if (IsValid(pCurWeapon))
	{
		if (func_Player_Bullet.IsBound())
			func_Player_Bullet.Broadcast(pCurWeapon->GetCurrentBullet(), pCurWeapon->GetMaxBullet());
		func_Player_Magazine.ExecuteIfBound(pCurWeapon->GetMagazine());
	}
	
}

void ATPSPortfolioCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Timer(DeltaSeconds);
	CameraControl(DeltaSeconds);
	UpdateState(DeltaSeconds);
}

ECharacterState ATPSPortfolioCharacter::GetCharacterState()
{
	if (stCharacterState == nullptr) return ECharacterState::IDLE;

	return stCharacterState->eState;
}

EWeaponType ATPSPortfolioCharacter::GetWeaponType()
{
	if (!IsValid(pCurWeapon)) return EWeaponType::WEAPON_NONE;

	return pCurWeapon->GetWeaponType();
}

FVector ATPSPortfolioCharacter::GetControlVector(bool IsFoward)
{
	if (Controller == nullptr)
		return vChangeDirection;
	
		// find out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	return IsFoward ?
		 FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X):
		 FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
}

FVector ATPSPortfolioCharacter::GetControlVectorLastUpdated(bool IsFoward)
{
	// get forward vector
	return IsFoward ?
		vControlVectorX :
		vControlVectorY;
}

float ATPSPortfolioCharacter::GetWalkSpeed()
{
	return GetCharacterMovement()->MaxWalkSpeed;
}

void ATPSPortfolioCharacter::SetWalkSpeed(float WalkSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ATPSPortfolioCharacter::SetBrakeSpeed(float BrakeSpeed)
{
	GetCharacterMovement()->BrakingDecelerationWalking = BrakeSpeed;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATPSPortfolioCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ATPSPortfolioCharacter::Evade);
	
		//Run
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &ATPSPortfolioCharacter::Run);

		//Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ATPSPortfolioCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ATPSPortfolioCharacter::StopSprint);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATPSPortfolioCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ATPSPortfolioCharacter::MoveComplete);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATPSPortfolioCharacter::Look);

		//Aiming
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ATPSPortfolioCharacter::Aim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ATPSPortfolioCharacter::AimComplete);
	
		//Attacking
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ATPSPortfolioCharacter::Attack);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &ATPSPortfolioCharacter::AttackComplete);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ATPSPortfolioCharacter::AttackStart);
		
		//Reloading
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ATPSPortfolioCharacter::Reload);
		//EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Completed, this, &ATPSPortfolioCharacter::AttackComplete);

		EnhancedInputComponent->BindAction(Weapon1Action, ETriggerEvent::Started, this, &ATPSPortfolioCharacter::WeaponChangePrimary);
		EnhancedInputComponent->BindAction(Weapon2Action, ETriggerEvent::Started, this, &ATPSPortfolioCharacter::WeaponChangeSecondary);

	}

}

void ATPSPortfolioCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		vControlVectorX = ForwardDirection;
		vControlVectorY = RightDirection;

		//움직임에 따른 회전 방향값 지정
		SetMoveDirection(ForwardDirection, RightDirection, MovementVector);
		SetIsMoving(true);
		ChangeState(bIsSprint ? ECharacterState::SPRINT : ECharacterState::RUN);
	}
}

void ATPSPortfolioCharacter::MoveComplete()
{
	SetIsMoving(false);
	if (nullptr == stCharacterState)return;
	if (stCharacterState->eState == ECharacterState::RUN ||
		stCharacterState->eState == ECharacterState::SPRINT)
	ChangeState(ECharacterState::IDLE);
}

void ATPSPortfolioCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);

		//if (bIsAiming)
		//{
		//	const FRotator rotControl = Controller->GetControlRotation();
		//	const FRotator rotControl_Pitch(rotControl.Pitch, 0, 0);
		//	fAimAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::UpVector.Dot(rotControl_Pitch.Vector()))) / 180.f;
		//}
	}
}

void ATPSPortfolioCharacter::Aim()
{
	if (bIsCrawltoIdle) {
		if(nullptr != stCharacterState &&
			stCharacterState->eState == ECharacterState::AIM)
		ChangeState(ECharacterState::IDLE);
		return;
	}
	ChangeState(ECharacterState::AIM);
}

void ATPSPortfolioCharacter::AimComplete()
{
	if(stCharacterState->eState != ECharacterState::EVADE)
	ChangeState(ECharacterState::IDLE);
}

void ATPSPortfolioCharacter::Attack()
{
	if (nullptr == pCurWeapon) return;
	if (bIsEquiping || bIsEvade) return;

	pCurWeapon->AttackStart();
	if (func_Player_Bullet.IsBound())
		func_Player_Bullet.Broadcast(pCurWeapon->GetCurrentBullet(), pCurWeapon->GetMaxBullet());
}

void ATPSPortfolioCharacter::AttackStart()
{
	if (nullptr == pCurWeapon) return;
	if (bIsEquiping) return;

	pCurWeapon->AttackStart();
	if (func_Player_Bullet.IsBound())
		func_Player_Bullet.Broadcast(pCurWeapon->GetCurrentBullet(), pCurWeapon->GetMaxBullet());

	if (bIsAiming && pCurWeapon->GetCurrentBullet() <= 0)
		Reload();
}

void ATPSPortfolioCharacter::AttackComplete()
{
	if (nullptr == pCurWeapon) return;
	UE_LOG(LogTemp, Log, TEXT("AttackStop"));
	pCurWeapon->AttackStop();
}

void ATPSPortfolioCharacter::Reload()
{
	if (nullptr == pCurWeapon) return;
	if (pCurWeapon->IsFullCapacity()) return;
	if (!pCurWeapon->IsPosibleReload()) return;

	bIsReloading = true;

	pCurWeapon->ReloadStart();
	if (func_Player_Bullet.IsBound())
		func_Player_Bullet.Broadcast(pCurWeapon->GetCurrentBullet(), pCurWeapon->GetMaxBullet());
}

void ATPSPortfolioCharacter::Evade()
{
	if (bIsCrawl)
	{
		if(!bIsCrawltoIdle)
			bIsCrawltoIdle = true;
		return;
	}

	ChangeState(ECharacterState::EVADE);
}

void ATPSPortfolioCharacter::EvadeComplete()
{
	if (nullptr == stCharacterState) return;
	if(stCharacterState->eState == ECharacterState::EVADE)
		ChangeState(ECharacterState::IDLE);
}

void ATPSPortfolioCharacter::WeaponChangePrimary()
{
	if (bIsEquiping) return;

	iWeaponIndex = 1;
	bIsEquiping = true;
}

void ATPSPortfolioCharacter::ReloadComplete()
{
	if (!bIsReloading) return;
	bIsReloading = false;
	if (nullptr == pCurWeapon) return;
	pCurWeapon->Reload();

	if (func_Player_Bullet.IsBound())
		func_Player_Bullet.Broadcast(pCurWeapon->GetCurrentBullet(), pCurWeapon->GetMaxBullet());
	func_Player_Magazine.ExecuteIfBound(pCurWeapon->GetMagazine());
}

void ATPSPortfolioCharacter::WeaponChangeSecondary()
{
	if (bIsEquiping) return;

	iWeaponIndex = 2;
	bIsEquiping = true;
}

void ATPSPortfolioCharacter::SetMoveDirection(const FVector& vFoward, const FVector& vRight, const FVector2D& vMoveVector)
{
	//움직임을 캐릭터 방향벡터를 기준으로 움직이도록 설정
	if (vMoveVector.X == 0 && vMoveVector.Y == 0) return;
	
	vChangeDirection = 
#pragma region LEFT / RIGHT	
		vMoveVector.Y == 0 ? vMoveVector.X > 0 ? vRight : -vRight :
#pragma endregion					
#pragma region FOWARD / FR / FL
		vMoveVector.Y > 0 ? vMoveVector.X == 0 ? vFoward : vMoveVector.X > 0 ? (vFoward + vRight).GetSafeNormal() : (vFoward - vRight).GetSafeNormal() :
#pragma endregion		
#pragma region BACK / BR / BL
		vMoveVector.X == 0 ? -vFoward : vMoveVector.X > 0 ? (-vFoward + vRight).GetSafeNormal() : (-vFoward - vRight).GetSafeNormal();
#pragma endregion

}

void ATPSPortfolioCharacter::Turn(float DeltaSeconds)
{
	if (!bIsMoving || bIsAiming) return;

	const FVector vActorForwardDirection = GetActorForwardVector();
	//내적으로 회전값 구하기

	double dArccosRadian = FMath::Acos(vActorForwardDirection.Dot(vChangeDirection));
	float fDegree = FMath::RadiansToDegrees(dArccosRadian);

	float fCurrentWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

	if (fDegree > MAX_TURN_DGREE && fCurrentWalkSpeed > RUN_CONDITION) SetBraking(fCurrentWalkSpeed > SPRINT_CONDITION ? BRAKE_SPRINT : BRAKE_RUN);

	//외적으로 회전방향 구하기
	if (vActorForwardDirection.Cross(vChangeDirection).Z < 0)
		fDegree *= -1;

	float fCalcTurnSpeed = fCurrentWalkSpeed > SPRINT_TURN_DCREASE ? fTurnSpeed / (fCurrentWalkSpeed / SPRINT_TURN_DCREASE) : fTurnSpeed;

	//선형보간으로 회전속도 조절
	double rTurnRot = FMath::FInterpConstantTo(0, fDegree, DeltaSeconds, fCalcTurnSpeed);


	//좌우 기울기 움직임 값 적용을 위한 벡터의 점진적 회전 적용
	float fLerpDegree = FMath::RadiansToDegrees(FMath::Acos(vLerpDirection.Dot(vChangeDirection)));
	if (vLerpDirection.Cross(vChangeDirection).Z < 0)
		fLerpDegree *= -1;
	double rLerpTurnRot = FMath::FInterpConstantTo(0, fLerpDegree, DeltaSeconds, fCalcTurnSpeed * 2.f);
	vLerpDirection = vLerpDirection.RotateAngleAxis(rLerpTurnRot, FVector::UpVector);

	fCrossAngle = vActorForwardDirection.Cross(vLerpDirection).Z;

	if (!bIsBraking)
		AddActorWorldRotation(FRotator(0, rTurnRot, 0));

}

void ATPSPortfolioCharacter::CameraControl(float DeltaSeconds)
{
	if (FollowCamera == nullptr)
		return;

	float fCurFOV = FollowCamera->FieldOfView;
	float fTargetFOV = bIsCrawl ? 100.f : bIsAiming ? 90.f : 100.f;
	float fLerpFOV = FMath::FInterpTo(fCurFOV, fTargetFOV, DeltaSeconds, CAMERA_FOV_SPEED);
	FollowCamera->SetFieldOfView(fLerpFOV);

	float fCurLength = CameraBoom->TargetArmLength;
	float fTargetLength = bIsCrawl ? 300.f : bIsAiming ? 200.f : 350.f;
	float fLerpLength = FMath::FInterpTo(fCurLength, fTargetLength, DeltaSeconds, CAMERA_FOV_SPEED);
	CameraBoom->TargetArmLength = fLerpLength;

	FVector vCurCamera = CameraBoom->SocketOffset;
	FVector vTarget = bIsCrawl ? FVector(0.f, 45.f, 30.f) : bIsAiming ? FVector(0.f, 60.f, 80.f) : FVector(0.f, 45.f ,70.f);
	FVector vLerp = FMath::VInterpTo(vCurCamera, vTarget, DeltaSeconds, CAMERA_FOV_SPEED);
	
	CameraBoom->SocketOffset = vLerp;

	//----------------------LayTrace-------------------------
	if (!bIsAiming)
	{
		fAimAngle = 0.f;
		return;
	}

	//카메라 컴포넌트포지션
	FVector vecWorldLocation = FollowCamera->GetComponentLocation();
	//카메라 로테이션
	FVector vecCameraLook = Controller->GetControlRotation().Vector();
	//타겟=포지션+방향*최대거리
	FVector vecTarget = vecWorldLocation + vecCameraLook * 3000.f;
	FHitResult FHresult;
	//콜리전파라미터에 자기자신 충돌무시
	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(FHresult, vecWorldLocation, vecTarget, ECollisionChannel::ECC_OverlapAll_Deprecated, collisionParams))
		vAimPosition = FHresult.Location;
	else
		vAimPosition = vecTarget;

	FVector vTargetPos = GetActorLocation();
	vTargetPos.Z += 60.f;
	const FRotator rotControl = (vAimPosition- vTargetPos).GetSafeNormal().Rotation();
	const FRotator rotControl_Pitch(rotControl.Pitch, 0, 0);
	const FVector vControlPitch = rotControl_Pitch.Vector();
	const FRotator rotActor = GetActorForwardVector().Rotation();
	const FRotator rotActor_Pitch(rotActor.Pitch, 0, 0);
	const FVector vActorPitch = rotActor_Pitch.Vector();
	fAimAngle = FMath::RadiansToDegrees(FMath::Acos(vActorPitch.Dot(vControlPitch)));
	if (vActorPitch.Cross(vControlPitch).Y < 0.f)
		fAimAngle *= -1.f;

	//DrawDebugLine(GetWorld(), vecWorldLocation, vecTarget, FColor::Red, false, 1.0f);
	//UE_LOG(LogTemp, Log, TEXT("fAimAngle : %f"), fAimAngle);
}

void ATPSPortfolioCharacter::CaculateTotalWalkSpeed(float DeltaSeconds)
{
	float fMaxSpeed = bIsBraking ? 0.f : !bIsMoving ? 0.f : bIsSprint ? fSprintSpeed : bIsRun ? fRunSpeed : fDefaultWalkSpeed;

	float fLerpSpeed = bIsBraking ? BRAKE_DECLEASE : !bIsMoving ? IDLE_DECLEASE : WALKSPEED_DECLEASE;
	GetCharacterMovement()->BrakingDecelerationWalking = bIsBraking ? BRAKE_DECLEASE : IDLE_DECLEASE;

	GetCharacterMovement()->MaxWalkSpeed = FMath::FInterpConstantTo(GetCharacterMovement()->MaxWalkSpeed, fMaxSpeed, DeltaSeconds, fLerpSpeed);

	//UE_LOG(LogTemp, Log, TEXT("MaxSpeed : %f, LerpSpeed : %f"), GetCharacterMovement()->MaxWalkSpeed, fLerpSpeed);
}

void ATPSPortfolioCharacter::SetAimRate(ECharacterState eChangeState)
{
	float fAimrate = 0.f;
	switch (eChangeState)
	{
	case ECharacterState::IDLE :
	case ECharacterState::BRAKE: fAimrate = 1.f;
		break;
	case ECharacterState::RUN: fAimrate = bIsAiming ? 0.5f : 2.f;
		break;
	case ECharacterState::SPRINT: fAimrate = bIsAiming ? 1.f : 3.f;
		break;

	case ECharacterState::AIM: fAimrate = 0.f;
		break;
	}
	if(func_Player_Aimrate.IsBound())
	func_Player_Aimrate.Broadcast(fAimrate);
}

void ATPSPortfolioCharacter::SetBraking(float fTime)
{
	ChangeState(ECharacterState::BRAKE);
	fBrakeTimer = fTime;
}

void ATPSPortfolioCharacter::NotifyEquip()
{
	switch (iWeaponIndex)
	{
	case 1: SetPrimaryEquip(); 
		break;
	case 2: SetSecondaryEquip();
		break;
	}
}

void ATPSPortfolioCharacter::SetPrimaryEquip()
{
	if (WeaponSlot.Num() < 1) return;

	if (IsValid(pCurWeapon))
	{
		pCurWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		pCurWeapon->SetHide(true);

		bool bIsSame = pCurWeapon == WeaponSlot[0];

		pCurWeapon = nullptr;
		
		if(bIsSame) return;
	}

	

	pCurWeapon = WeaponSlot[0];

	FName WeaponSocket = TEXT("r_hand_rifle");
	
	if (GetMesh()->DoesSocketExist(WeaponSocket) && pCurWeapon != nullptr)
	{
		pCurWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		pCurWeapon->SetHide(false);
	}

	if (IsValid(pCurWeapon))
	{
		if (func_Player_Bullet.IsBound())
			func_Player_Bullet.Broadcast(pCurWeapon->GetCurrentBullet(), pCurWeapon->GetMaxBullet());
		func_Player_Magazine.ExecuteIfBound(pCurWeapon->GetMagazine());
	}
}

void ATPSPortfolioCharacter::SetSecondaryEquip()
{
	if (WeaponSlot.Num() < 2) return;

	if (IsValid(pCurWeapon))
	{
		pCurWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		pCurWeapon->SetHide(true);
		
		bool bIsSame = pCurWeapon == WeaponSlot[1];
		
		pCurWeapon = nullptr;

		if(bIsSame) return;
	}

	

	pCurWeapon = WeaponSlot[1];

	FName WeaponSocket = TEXT("r_hand_rifle");

	if (GetMesh()->DoesSocketExist(WeaponSocket) && pCurWeapon != nullptr)
	{
		pCurWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		pCurWeapon->SetHide(false);
	}

	if (IsValid(pCurWeapon))
	{
		if (func_Player_Bullet.IsBound())
			func_Player_Bullet.Broadcast(pCurWeapon->GetCurrentBullet(), pCurWeapon->GetMaxBullet());
		func_Player_Magazine.ExecuteIfBound(pCurWeapon->GetMagazine());
	}
}

void ATPSPortfolioCharacter::SetJumpAway()
{
	SetIsCrawl(true);
	FVector vImpulse = stCharacterState->LastDirection();
	vImpulse.Z += 0.4f;
	GetCharacterMovement()->AddImpulse(vImpulse * 600.f, true);
}

void ATPSPortfolioCharacter::Timer(float DeltaSeconds)
{
	//Timer
	if (fBrakeTimer > 0.f) fBrakeTimer -= DeltaSeconds;
	else if (bIsBraking)
	{
		ChangeState(ECharacterState::IDLE);
	}
}

void ATPSPortfolioCharacter::SlideGround(float DeltaSeconds)
{

}

void ATPSPortfolioCharacter::UpdateState(float DeltaSeconds)
{
	if (stCharacterState == nullptr) return;

	stCharacterState->Update(DeltaSeconds);
}

void ATPSPortfolioCharacter::ChangeState(ECharacterState eChangeState)
{
	if (stCharacterState == nullptr || stCharacterState->eState == eChangeState) return;

	SetAimRate(eChangeState);

	if (!CanChangeState(eChangeState)) return;

	for (int i = 0; i < vecState.size(); ++i)
	{
		if (vecState[i]==nullptr || eChangeState != vecState[i]->eState)
			continue;

		stCharacterState->Exit();
		stCharacterState = vecState[i].get();
		stCharacterState->Enter();
		break;
	}

	UE_LOG(LogTemp, Log, TEXT("ChangeState : %i"), eChangeState);
}

bool ATPSPortfolioCharacter::CanChangeState(ECharacterState changestate)
{
	if (stCharacterState == nullptr || stCharacterState->eState == changestate) return false;

	switch (stCharacterState->eState)
	{
	case ECharacterState::BRAKE :
	case ECharacterState::AIM :
		if (changestate != ECharacterState::IDLE && changestate != ECharacterState::EVADE) return false;
		break;

	case ECharacterState::EVADE :
		if (changestate != ECharacterState::IDLE && changestate != ECharacterState::AIM) return false;
		if (changestate == ECharacterState::AIM && !bIsCrawl) return false;
		break;
	}
		
	return true;
}

void ATPSPortfolioCharacter::PlayAttack(bool ismelee)
{
	if (bIsCrawl) return;
	
	bIsAttacking = true;

	auto pMesh = GetMesh();
	if (!IsValid(pMesh)) return;
	auto pAnimInst = pMesh->GetAnimInstance();
	if (!IsValid(pAnimInst)) return;
	
	auto eType = pCurWeapon == nullptr ? EWeaponType::WEAPON_NONE : pCurWeapon->GetWeaponType();
	auto tpsAnimInst = Cast<UTPSAnimInstance>(pAnimInst);
	if(IsValid(tpsAnimInst))
		tpsAnimInst->PlayAttack(eType, ismelee);
}

void ATPSPortfolioCharacter::SetCrawlEnd()
{
	bIsCrawl = false;
	bIsCrawltoIdle = false;
}


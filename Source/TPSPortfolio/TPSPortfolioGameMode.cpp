// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSPortfolioGameMode.h"
#include "TPSPortfolioCharacter.h"
#include "TPSPlayerController.h"
#include "UObject/ConstructorHelpers.h"

ATPSPortfolioGameMode::ATPSPortfolioGameMode()
{
	// set default pawn class to our Blueprinted character
	///Script/CoreUObject.Class'/Script/TPSPortfolio.TPSPortfolioCharacter'
	///Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"
	/*static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}*/
	DefaultPawnClass = ATPSPortfolioCharacter::StaticClass();
	PlayerControllerClass = ATPSPlayerController::StaticClass();
}

// Copyright Epic Games, Inc. All Rights Reserved.


#include "NetTPSPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "NetTPS.h"
#include "Widgets/Input/SVirtualJoystick.h"

void ANetTPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 로컬 플레이어 컨트롤러에만 터치 UI를 생성합니다.
	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogNetTPS, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
}

void ANetTPSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 로컬 플레이어 컨트롤러에만 입력 매핑 컨텍스트를 추가합니다.
	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// 터치 입력을 쓰지 않을 때만 키보드/마우스 전용 매핑을 추가합니다.
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

bool ANetTPSPlayerController::ShouldUseTouchControls() const
{
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

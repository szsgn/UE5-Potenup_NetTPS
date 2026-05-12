// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NetTPSPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;

// Enhanced Input과 선택적 터치 컨트롤을 설정하는 플레이어 컨트롤러입니다.
UCLASS(abstract)
class ANetTPSPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	/** 기본 입력 매핑 컨텍스트입니다. */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** 터치 컨트롤이 활성화되면 제외할 입력 매핑 컨텍스트입니다. */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** 모바일 컨트롤 위젯 클래스입니다. */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** 생성된 모바일 컨트롤 위젯입니다. */
	UPROPERTY()
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** 모바일 플랫폼이 아니어도 UMG 터치 컨트롤을 강제로 사용합니다. */
	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	/** UMG 터치 컨트롤을 사용할지 반환합니다. */
	bool ShouldUseTouchControls() const;
};

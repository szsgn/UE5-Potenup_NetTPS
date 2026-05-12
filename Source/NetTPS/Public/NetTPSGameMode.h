// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NetTPSGameMode.generated.h"

// TPS 테스트 맵에서 사용하는 기본 게임 모드입니다.
UCLASS(abstract)
class ANetTPSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANetTPSGameMode();
};

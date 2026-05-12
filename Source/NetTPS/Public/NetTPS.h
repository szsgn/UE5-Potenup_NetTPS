// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/** 프로젝트 공용 로그 카테고리입니다. */
DECLARE_LOG_CATEGORY_EXTERN(LogNetTPS, Log, All);

// 네트워크 디버그 로그에서 현재 액터의 Role과 NetMode를 짧게 출력하기 위한 헬퍼입니다.
#define LOCALROLE (UEnum::GetValueAsString<ENetRole>(GetLocalRole()))
#define REMOTEROLE (UEnum::GetValueAsString<ENetRole>(GetRemoteRole()))

#define NETMODE (GetWorld()->GetNetMode()==ENetMode::NM_Client?TEXT("CLIENT") : GetWorld()->GetNetMode()==NM_Standalone?TEXT("Standalone"):TEXT("Server"))

#define CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define PRINTLOG(fmt, ...) UE_LOG(LogNetTPS, Warning, TEXT("[%s]%s : %s"),NETMODE, *CALLINFO, *FString::Printf(fmt, ##__VA_ARGS__))															

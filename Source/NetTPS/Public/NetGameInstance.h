#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "NetGameInstance.generated.h"

// 세션 검색 결과를 UI에 넘기기 위한 표시용 데이터입니다.
USTRUCT(BlueprintType)
struct FSessionInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString roomName;

	UPROPERTY(BlueprintReadOnly)
	FString hostName;

	UPROPERTY(BlueprintReadOnly)
	FString playerCount;

	UPROPERTY(BlueprintReadOnly)
	int32 pingSpeed;

	UPROPERTY(BlueprintReadOnly)
	int32 index;

	FString ToString() const
	{
		return FString::Printf(TEXT("[%d]%s : %s - %s, %dms"), index, *roomName, *hostName, *playerCount, pingSpeed);
	}
};

// 세션 검색 결과를 하나씩 전달합니다.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSearchSignature, const FSessionInfo&, sessionInfo);

// 세션 검색 진행 상태를 전달합니다.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSearchStateSignature, bool, bIsSearching);

// 온라인 세션 생성, 검색, 입장, 종료 흐름을 관리합니다.
UCLASS()
class NETTPS_API UNetGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	IOnlineSessionPtr sessionInterface;

	// 세션 생성
	void CreateSession(FString roomName, int32 playerCount);

	// 현재 플레이어가 호스트하거나 입장할 때 사용하는 세션 이름
	FString mySessionName = "Technoise";

	UFUNCTION()
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	void GameToStart();

	// 세션 검색
	TSharedPtr<FOnlineSessionSearch> sessionSearch;

	void FindOtherSessions();
	void OnFindSessionsComplete(bool bWasSuccessful);

	FSearchSignature onSearchCompleted;
	FSearchStateSignature onSearchState;

	// 세션 입장
	void JoinSelectedSession(int32 index);
	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);

	// 세션 종료 및 실패 처리
	void ExitRoom();
	void OnDestroySessionComplete(FName sessionName, bool bWasSuccessful);
	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);

	bool IsInRoom();

	// Steam 세션 메타데이터에서 한글이 깨지는 문제를 피하기 위해 문자열을 Base64로 변환합니다.
	FString StringBase64Encode(const FString& str);
	FString StringBase64Decode(const FString& str);
};

#pragma once

#include "CoreMinimal.h"
#include "NetTPSPlayerController.h"
#include "NetPlayerController.generated.h"

class ANetTPSGameMode;
class UMainUI;

// 전투 맵에서 리스폰, 관전 전환, 메인 UI 생성을 담당하는 플레이어 컨트롤러입니다.
UCLASS()
class NETTPS_API ANetPlayerController : public ANetTPSPlayerController
{
	GENERATED_BODY()

private:
	UPROPERTY()
	ANetTPSGameMode* gm;

public:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void ServerRPC_RespawnPlayer();

	UFUNCTION(Server, Reliable)
	void ServerRPC_ChangeToSpectator();

	// 메인 UI 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UMainUI> mainUIWidget;

	// 생성된 메인 UI 인스턴스
	UPROPERTY()
	UMainUI* mainUI;
};

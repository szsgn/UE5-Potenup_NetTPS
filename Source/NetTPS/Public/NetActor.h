#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetActor.generated.h"

class UMaterialInstanceDynamic;
class UStaticMeshComponent;

// 네트워크 소유권, 회전 보간, 머티리얼 색상 동기화를 테스트하는 액터입니다.
UCLASS()
class NETTPS_API ANetActor : public AActor
{
	GENERATED_BODY()

public:
	ANetActor();

	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 네트워크 상태 디버그 출력
	void PrintNetLog();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* meshComp;

	// Owner 탐색 범위
	UPROPERTY(EditAnywhere)
	float searchDistance = 200.0f;

	// 탐색 범위 안의 플레이어를 Owner로 설정
	void FindOwner();

	// 서버 회전 값을 클라이언트에 동기화
	UPROPERTY(ReplicatedUsing=OnRep_RotYaw)
	float RotYaw = 0.0f;

	UFUNCTION()
	void OnRep_RotYaw();

	float currentTime;
	float lastTime;

	UPROPERTY()
	UMaterialInstanceDynamic* Mat;

	// 동기화할 머티리얼 색상
	UPROPERTY(ReplicatedUsing=OnRep_ChangeMatColor)
	FLinearColor MatColor;

	UFUNCTION()
	void OnRep_ChangeMatColor();

	UFUNCTION(Server, Reliable)
	void ServerRPC_ChangeColor(const FLinearColor newColor);

	UFUNCTION(Client, Unreliable)
	void ClientRPC_ChangeColor(const FLinearColor newColor);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPC_ChangeColor(const FLinearColor newColor);

	FTimerHandle handle;
};

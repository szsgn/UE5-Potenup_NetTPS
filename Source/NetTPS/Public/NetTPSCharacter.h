// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NetTPSCharacter.generated.h"

class UCameraComponent;
class UCameraShakeBase;
class UInputAction;
class UInputComponent;
class UMainUI;
class UParticleSystem;
class USceneComponent;
class USpringArmComponent;
class UWidgetComponent;
struct FHitResult;
struct FInputActionValue;

// 조작, 권총, 체력, UI, RPC 테스트를 담당하는 네트워크 TPS 캐릭터입니다.
UCLASS(abstract)
class ANetTPSCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	// 카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

protected:
	// 입력 액션
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

public:
	ANetTPSCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostNetInit() override;
	virtual void PossessedBy(AController* NewController) override;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

public:
	// 입력 처리
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// 권총 장착 상태
	UPROPERTY(VisibleAnywhere)
	USceneComponent* GunComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* TakePistolAction;

	UPROPERTY(Replicated)
	bool bHasPistol = false;

	UPROPERTY(Replicated)
	AActor* ownedPistol = nullptr;

	UPROPERTY(EditAnywhere, Category="Gun")
	float DistanceToGun = 200.0f;

	UPROPERTY()
	TArray<AActor*> pistolActors;

	void TakePistol(const FInputActionValue& Value);
	void AttachPistol(AActor* pistolActor);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* ReleaseAction;

	void ReleasePistol(const FInputActionValue& Value);
	void DetachPistol(AActor* pistolActor);

	// 사격
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* FireAction;

	UPROPERTY(EditDefaultsOnly, Category="Gun")
	UParticleSystem* GunEffect;

	void Fire(const FInputActionValue& Value);

	// 플레이어 UI
	UPROPERTY()
	UMainUI* mainUI;

	void InitUIWidget();

	UFUNCTION(Client, Reliable)
	void ClientRPC_InitUIWidget();

	// 탄약
	UPROPERTY(EditAnywhere, Category="Bullet")
	int32 MaxBulletCount = 10;

	UPROPERTY(ReplicatedUsing=OnRep_BulletCount)
	int32 BulletCount = MaxBulletCount;

	UFUNCTION()
	void OnRep_BulletCount();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* ReloadAction;

	void ReloadPistol(const FInputActionValue& Value);
	void InitAmmoUI();

	bool IsReloading = false;

	// 체력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="HP")
	float MaxHP = 3.0f;

	UPROPERTY(ReplicatedUsing=OnRep_HP, BlueprintReadOnly, Category="HP")
	float hp = MaxHP;

	UFUNCTION()
	void OnRep_HP();

	__declspec(property(get=GetHP, put=SetHP)) float HP;
	float GetHP();
	void SetHP(float value);

	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* hpUIComp;

	void DamageProcess();

	bool isDead = false;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UCameraShakeBase> damageCameraShake;

	void DieProcess();

	// 디버그
	void PrintNetLog();

	// 권총 RPC
	UFUNCTION(Server, Reliable)
	void ServerRPC_TakePistol();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_TakePistol(AActor* pistolActor);

	UFUNCTION(Server, Reliable)
	void ServerRPC_ReleasePistol();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_ReleasePistol(AActor* pistolActor);

	UFUNCTION(Server, Reliable)
	void ServerRPC_Fire();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPC_Fire(bool bHit, const FHitResult& hitInfo);

	UFUNCTION(Server, Reliable)
	void ServerRPC_Reload();

	UFUNCTION(Client, Reliable)
	void ClientRPC_Reload();

	// 음성 채팅
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	UInputAction* voiceAction;

	void StartVoiceChat();
	void StopVoiceChat();

	// 텍스트 채팅
	UFUNCTION(Server, Reliable)
	void ServerRPC_SendMsg(const FString& msg);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_SendMsg(const FString& msg);
};

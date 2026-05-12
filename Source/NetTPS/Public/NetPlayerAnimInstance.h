#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NetPlayerAnimInstance.generated.h"

class ANetTPSCharacter;
class UAnimMontage;

// 네트워크 캐릭터의 이동, 무기, 피격 애니메이션 상태를 갱신합니다.
UCLASS()
class NETTPS_API UNetPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MyAnimSettings")
	bool bHasPistol = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MyAnimSettings")
	float myDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MyAnimSettings")
	float mySpeed;

	UPROPERTY()
	ANetTPSCharacter* player;

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// 사격 몽타주
	UPROPERTY(EditDefaultsOnly, Category="Anim")
	UAnimMontage* FireMontage;

	// 사격 애니메이션 재생
	void PlayFireAnimation();

	// 카메라 피치에 맞춰 상체를 조준하기 위한 값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MyAnimSettings")
	float pitchAngle;

	// 재장전 몽타주
	UPROPERTY(EditDefaultsOnly, Category="Anim")
	UAnimMontage* ReloadMontage;

	// 재장전 애니메이션 재생
	void PlayReloadAnimation();

	// 재장전 완료 노티파이
	UFUNCTION()
	void AnimNotify_OnReloadFinish();

	// 사망 애니메이션 종료 노티파이
	UFUNCTION()
	void AnimNotify_DieEnd();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MyAnimSettings")
	bool isDead = false;
};

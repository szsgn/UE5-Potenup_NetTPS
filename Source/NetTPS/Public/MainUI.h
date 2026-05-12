#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUI.generated.h"

class UButton;
class UChatWidget;
class UEditableText;
class UHorizontalBox;
class UImage;
class UScrollBox;
class UTextBlock;
class UUniformGridPanel;
class UWidgetAnimation;

// 전투 화면의 조준점, 탄약, 체력, 게임오버, 채팅 UI를 관리합니다.
UCLASS()
class NETTPS_API UMainUI : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// 조준점
	UPROPERTY(BlueprintReadWrite, Category="UI", meta=(BindWidget))
	UImage* img_Crosshair;

	void ShowCrosshair(bool isShow);

	// 탄약 UI
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UUniformGridPanel* BulletPanel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Bullet")
	TSubclassOf<UUserWidget> BulletUIFactory;

	void AddBullet();
	void PopBullet(int32 index);
	void RemoveAllAmmo();

	// 체력 UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="HP")
	float HP = 1.0f;

	UPROPERTY(EditDefaultsOnly, meta=(BindWidgetAnim), Transient, Category="MySettings")
	UWidgetAnimation* DamageAnim;

	void PlayDamageAnimation();

	// 게임오버 UI
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UHorizontalBox* GameoverUI;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UButton* btn_retry;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UButton* btn_exit;

	UFUNCTION()
	void OnRetry();

	UFUNCTION()
	void OnExit();

	// 접속자 목록
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* txt_users;

	// 채팅 UI
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UChatWidget> chatWidget;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UScrollBox* scroll_msgList;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UEditableText* edit_input;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UButton* btn_send;

	UFUNCTION()
	void SendMsg();

	void ReceiveMsg(const FString& msg);
};

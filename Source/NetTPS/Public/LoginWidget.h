#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoginWidget.generated.h"

class UButton;
class UEditableText;
class UNetGameInstance;
class UScrollBox;
class USessionSlotWidget;
class USlider;
class UTextBlock;
class UWidgetSwitcher;
struct FSessionInfo;

// 로비 화면에서 방 생성, 세션 검색, 입장 준비 UI를 관리합니다.
UCLASS()
class NETTPS_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// 메인 패널
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UButton* btn_createRoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UEditableText* edit_roomName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget))
	USlider* slider_playerCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UTextBlock* txt_playerCount;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UWidgetSwitcher* WidgetSwitcher;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UButton* btn_createSession;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UButton* btn_findSession;

	UFUNCTION()
	void SwitchCreatePanel();

	UFUNCTION()
	void SwitchFindPanel();

	// 공통 뒤로가기
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UButton* btn_back;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UButton* btn_back_1;

	UFUNCTION()
	void BackToMain();

	// 세션 검색
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UButton* btn_find;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UTextBlock* txt_findingMsg;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UEditableText* edit_userName;

	UFUNCTION()
	void OnClickedFindSession();

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UButton* btn_GameToStart;

	UFUNCTION()
	void OnClickedGameToStart();

	// 세션 슬롯 목록
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	UScrollBox* scroll_roomList;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USessionSlotWidget> sessionInfoWidget;

	UFUNCTION()
	void AddSlotWidget(const FSessionInfo& sessionInfo);

	// 세션 검색 상태 변경 콜백
	UFUNCTION()
	void OnChangeButtonEnable(bool bIsSearching);

	UPROPERTY()
	UNetGameInstance* gi;

	UFUNCTION()
	void CreateRoom();

	UFUNCTION()
	void OnValueChanged(float Value);
};

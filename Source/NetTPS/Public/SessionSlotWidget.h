#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionSlotWidget.generated.h"

class UButton;
class UTextBlock;
struct FSessionInfo;

// 검색된 세션 하나를 표시하고 입장 요청을 전달하는 위젯입니다.
UCLASS()
class NETTPS_API USessionSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* txt_roomName;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* txt_hostName;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* txt_playerCount;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* txt_pingSpeed;

	int32 sessionNumber;

	void Set(const FSessionInfo& sessionInfo);

	// 세션 입장
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UButton* btn_join;

	UFUNCTION()
	void JoinSession();

	virtual void NativeConstruct() override;
};

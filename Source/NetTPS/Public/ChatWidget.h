#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatWidget.generated.h"

class UTextBlock;

// 채팅 메시지 한 줄을 표시하는 위젯입니다.
UCLASS()
class NETTPS_API UChatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* txt_msg;
};

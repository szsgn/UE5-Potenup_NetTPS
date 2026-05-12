#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

// 캐릭터 머리 위에 표시되는 체력 바 위젯입니다.
UCLASS()
class NETTPS_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="HP")
	float HP = 1.0f;
};

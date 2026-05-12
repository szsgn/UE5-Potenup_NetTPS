#include "NetActor.h"

#include "EngineUtils.h"
#include "NetTPS.h"
#include "NetTPSCharacter.h"
#include <Net/UnrealNetwork.h>

#include "NetGameInstance.h"

ANetActor::ANetActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	// 대역폭 조정
	SetNetUpdateFrequency(1.0f);
	
	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = meshComp;
	meshComp->SetRelativeScale3D(FVector(0.5f));
}

void ANetActor::BeginPlay()
{
	Super::BeginPlay();
	
	Mat = meshComp->CreateDynamicMaterialInstance(0);
	if ( HasAuthority() )
	{
		auto gi = GetGameInstance<UNetGameInstance>();
		
		GetWorldTimerManager().SetTimer(handle, 
			FTimerDelegate::CreateLambda([&, gi]
			{
				if ( gi->IsInRoom() )
				{
					FLinearColor MatColor = FLinearColor(FMath::RandRange(0.0f, 0.3f), FMath::RandRange(0.0f, 0.3f), FMath::RandRange(0.0f, 0.3f), 1.0f);
					ServerRPC_ChangeColor(MatColor);					
				}
			}), 1, true);
	}
	
}

void ANetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PrintNetLog();
	
	FindOwner();
	
	
	// 서버는 실제 회전 값을 갱신하고 클라이언트는 마지막 동기화 값으로 보간합니다.
	if ( HasAuthority() )
	{
		AddActorLocalRotation(FRotator(0.0f, 50.0f * DeltaTime, 0.0f));
		RotYaw = GetActorRotation().Yaw;
	}
	else
	{
		// 경과 시간 증가
		currentTime += DeltaTime;
		
		// 0으로 나눠지지 않도록 lastTime 값을 확인합니다.
		if ( lastTime < KINDA_SMALL_NUMBER)
		{
			return;
		}
		
		// 이전 동기화 간격 대비 현재 진행 비율을 계산합니다.
		float lerpRatio = currentTime / lastTime;

		// 이전 동기화 간격만큼 더 회전했을 값을 예측합니다.
		float newYaw = RotYaw + 50.0f * lastTime;

		// 예측 값까지 현재 진행 비율만큼 보간합니다.
		float lerpYaw = FMath::Lerp(RotYaw, newYaw, lerpRatio);
		
		FRotator CurRot = GetActorRotation();
		CurRot.Yaw = lerpYaw;
		SetActorRotation(CurRot);
		
		
		
		
		
		
		
		
		
		
	}	
	
	
	
	DrawDebugSphere(GetWorld(), GetActorLocation(), searchDistance, 30, FColor::Yellow, false, 0, 0, 1);	
}

void ANetActor::PrintNetLog()
{
	const FString conStr = GetNetConnection() != nullptr ? TEXT("Valid Connection") : TEXT("Invalid Connection");
		
	const FString ownerName = GetOwner() != nullptr ? GetOwner()->GetName() : TEXT("No Owner");
	
	const FString logStr = FString::Printf(TEXT("Connection : %s\nLocalRole : %s\nRemoteRole : %s\nOwnerName : %s"), *conStr, *LOCALROLE, *REMOTEROLE, *ownerName);
	
	DrawDebugString( GetWorld(), GetActorLocation() + FVector::UpVector * 100.0f, logStr, nullptr, FColor::White, 0, true, 1);
}

void ANetActor::FindOwner()
{
	if ( HasAuthority())
	{
		AActor* newOwner = nullptr;
		float minDist = searchDistance;
		
		for ( TActorIterator<ANetTPSCharacter> it(GetWorld()); it; ++it)
		{
			AActor* otherActor = *it;
			float dist = GetDistanceTo(otherActor);
			
			if ( dist < minDist )
			{
				minDist = dist;
				newOwner = otherActor;
			}
		}
		
		// 탐색된 플레이어를 Owner로 설정합니다.
		if ( GetOwner() != newOwner )
		{
			SetOwner(newOwner);			
		}			
	}
}

void ANetActor::OnRep_RotYaw()
{
	FRotator NewRot = GetActorRotation();
	NewRot.Yaw = RotYaw;
	SetActorRotation(NewRot);
	// 마지막 동기화 간격을 저장한 뒤 누적 시간을 초기화합니다.
	lastTime = currentTime;
	currentTime = 0.0f;
}

void ANetActor::OnRep_ChangeMatColor()
{
	if ( Mat )
	{
		Mat->SetVectorParameterValue(TEXT("FloorColor"), MatColor);
	}
}

void ANetActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	GetWorld()->GetTimerManager().ClearTimer(handle);
}

void ANetActor::MulticastRPC_ChangeColor_Implementation(
	const FLinearColor newColor)
{
	if ( Mat )
	{
		Mat->SetVectorParameterValue(TEXT("FloorColor"), newColor);
	}
}

void ANetActor::ClientRPC_ChangeColor_Implementation(
	const FLinearColor newColor)
{
	if ( Mat )
	{
		Mat->SetVectorParameterValue(TEXT("FloorColor"), newColor);
	}
}

void ANetActor::ServerRPC_ChangeColor_Implementation(
	const FLinearColor newColor)
{
	MulticastRPC_ChangeColor(newColor);
}

void ANetActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ANetActor, RotYaw);
	DOREPLIFETIME(ANetActor, MatColor);
}



















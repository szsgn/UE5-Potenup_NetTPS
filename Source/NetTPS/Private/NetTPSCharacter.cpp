// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetTPSCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HealthBar.h"
#include "InputActionValue.h"
#include "MainUI.h"
#include "NetPlayerAnimInstance.h"
#include "NetPlayerController.h"
#include "NetPlayerState.h"
#include "NetTPS.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ANetTPSCharacter::ANetTPSCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);
	
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// 컨트롤러 회전은 카메라에만 반영합니다.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// TPS 조작에 맞게 캐릭터 이동을 설정합니다.
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	// 이동 튜닝 값은 캐릭터 블루프린트에서 빠르게 조정할 수 있습니다.
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// 카메라 붐
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 150.0f;
	CameraBoom->SetRelativeLocation(FVector(0.0f, 40.0f, 60.0f));
	CameraBoom->bUsePawnControlRotation = true;

	// 추적 카메라
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Mesh와 Anim Blueprint 참조는 파생 블루프린트에서 설정합니다.

	GunComp = CreateDefaultSubobject<USceneComponent>(TEXT("GunComp"));
	GunComp->SetupAttachment(GetMesh(), TEXT("HandGrip_R"));
	GunComp->SetRelativeLocation(FVector(0.824356f,8.975311f,3.870210f));
	GunComp->SetRelativeRotation(FRotator(0.0f, 0.0f, 12.399401f));


	// 체력 바 위젯 컴포넌트
	hpUIComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	hpUIComp->SetupAttachment(GetMesh());








	
	
	
}

void ANetTPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// 총 검색
	TArray<AActor*> allActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), allActors);
	for ( auto tempPistol : allActors )
	{
		if ( tempPistol->GetName().Contains("BP_Pistol") )
		{
			pistolActors.Add(tempPistol);
		}
	}
	
}

void ANetTPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// 입력 액션 바인딩
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// 점프
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// 이동
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANetTPSCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ANetTPSCharacter::Look);

		// 시점
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANetTPSCharacter::Look);
		
		// 권총 줍기
		EnhancedInputComponent->BindAction(TakePistolAction, ETriggerEvent::Started, this, &ANetTPSCharacter::TakePistol);
		
		// 권총 버리기
		EnhancedInputComponent->BindAction(ReleaseAction, ETriggerEvent::Started, this, &ANetTPSCharacter::ReleasePistol);
		
		// 사격
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ANetTPSCharacter::Fire);
		
		// 재장전
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ANetTPSCharacter::ReloadPistol);
		
		// 음성 채팅
		EnhancedInputComponent->BindAction(voiceAction, ETriggerEvent::Started, this, &ANetTPSCharacter::StartVoiceChat);
		EnhancedInputComponent->BindAction(voiceAction, ETriggerEvent::Completed, this, &ANetTPSCharacter::StopVoiceChat);
		
	}
	else
	{
		UE_LOG(LogNetTPS, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ANetTPSCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	DoMove(MovementVector.X, MovementVector.Y);
}

void ANetTPSCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ANetTPSCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ANetTPSCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ANetTPSCharacter::DoJumpStart()
{
	Jump();
}

void ANetTPSCharacter::DoJumpEnd()
{
	StopJumping();
}

void ANetTPSCharacter::PostNetInit()
{
	Super::PostNetInit();
	
	if ( bHasPistol && ownedPistol )
	{
		AttachPistol(ownedPistol);
	}
}

void ANetTPSCharacter::TakePistol(const FInputActionValue& Value)
{
	// 이미 권총을 들고 있으면 처리하지 않습니다.
	
	if ( bHasPistol )
	{
		return;
	}
	
	ServerRPC_TakePistol();	
}

void ANetTPSCharacter::AttachPistol(AActor* pistolActor)
{
	if ( pistolActor == nullptr )
	{
		return;
	}
	
	auto meshComp = pistolActor->GetComponentByClass<UStaticMeshComponent>();
	meshComp->SetSimulatePhysics(false);
	meshComp->AttachToComponent(GunComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	
	if ( IsLocallyControlled() && mainUI )
	{
		mainUI->ShowCrosshair(true);		
	}
}

void ANetTPSCharacter::ReleasePistol(const FInputActionValue& Value)
{
	// 권총을 들고 있지 않거나 재장전 중이면 처리하지 않습니다.
	if ( !bHasPistol || IsReloading || IsLocallyControlled() == false )
	{
		return;
	}
	
	ServerRPC_ReleasePistol();	
}

void ANetTPSCharacter::DetachPistol(AActor* pistolActor)
{
	auto meshComp = pistolActor->GetComponentByClass<UStaticMeshComponent>();
	meshComp->SetSimulatePhysics(true);
	meshComp->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	
	if ( IsLocallyControlled() && mainUI )
	{
		mainUI->ShowCrosshair(false);		
	}
}

void ANetTPSCharacter::Fire(const FInputActionValue& Value)
{	
	// 권총을 들고 있지 않거나 탄약이 없으면 처리하지 않습니다.
	if ( !bHasPistol || BulletCount <= 0 || IsReloading )
	{
		return;
	}		
	
	ServerRPC_Fire();
	
	
	
	
	
	
	
	
	
	
	
	
}

void ANetTPSCharacter::InitUIWidget()
{
	PRINTLOG(TEXT("[%s] Begin"), Controller ? TEXT("PLAYER") : TEXT("Not Player"));
	
	// 플레이어 컨트롤러가 없으면 UI를 생성하지 않습니다.
	auto pc = Cast<ANetPlayerController>(Controller);
	if ( pc == nullptr )
	{
		return;
	}
	
	if ( pc->mainUIWidget )
	{
		if ( pc->mainUI == nullptr )
		{
			pc->mainUI = Cast<UMainUI>(CreateWidget(GetWorld(), pc->mainUIWidget));			
		}
		mainUI = pc->mainUI;
		mainUI->AddToViewport();
		mainUI->ShowCrosshair(false);
		
		hp = MaxHP;
		mainUI->HP = 1.0f;
		
		// 기존 탄약 UI를 초기화합니다.
		mainUI->RemoveAllAmmo();
		
		BulletCount = MaxBulletCount;
		for ( int i = 0; i < BulletCount; ++i )
		{
			mainUI->AddBullet();
		}
		
		// 로컬 메인 UI가 있으므로 머리 위 체력 바는 숨깁니다.
		if ( hpUIComp )
		{
			hpUIComp->SetVisibility(false);
		}
	}	
}

void ANetTPSCharacter::ClientRPC_InitUIWidget_Implementation()
{
	if ( IsLocallyControlled() )
	{
		InitUIWidget();		
	}
}

void ANetTPSCharacter::OnRep_BulletCount()
{
	if ( mainUI )
	{
		mainUI->PopBullet(BulletCount);			
	}
}

void ANetTPSCharacter::ReloadPistol(const FInputActionValue& Value)
{
	// 권총을 들고 있지 않거나 이미 재장전 중이면 처리하지 않습니다.
	if ( !bHasPistol || IsReloading )
	{
		return;
	}
	
	// 재장전 애니메이션 재생
	auto anim = Cast<UNetPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	anim->PlayReloadAnimation();
	
	IsReloading = true;
}

void ANetTPSCharacter::InitAmmoUI()
{
	ServerRPC_Reload();	
}

void ANetTPSCharacter::OnRep_HP()
{
	// 체력이 0 이하가 되면 사망 상태로 전환합니다.
	if ( HP <= 0.0f )
	{
		isDead = true;
		ReleasePistol(FInputActionValue());
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCharacterMovement()->DisableMovement();
	}	
	
	// UI에 반영할 체력 비율을 계산합니다.
	float percent = hp / MaxHP;
	
	if ( mainUI )
	{
		mainUI->HP = percent;
		// 로컬 플레이어 UI와 피격 효과를 갱신합니다.
		mainUI->PlayDamageAnimation();
		
		if ( damageCameraShake )
		{
			auto pc = Cast<APlayerController>(Controller);
			if ( pc )
			{
				pc->ClientStartCameraShake(damageCameraShake);
			}
		}
	}
	else
	{
		auto hpUI = Cast<UHealthBar>(hpUIComp->GetWidget());
		if ( hpUI )
		{
			hpUI->HP = percent;			
		}
	}	
}

float ANetTPSCharacter::GetHP()
{
	return hp;
}

void ANetTPSCharacter::SetHP(float value)
{
	hp = value;	
	OnRep_HP();
}

void ANetTPSCharacter::DamageProcess()
{
	// 체력을 감소시킵니다.
	HP -= 1.0f;	
	
	// 사망 상태를 갱신합니다.
	if ( HP <= 0.0f )
	{
		isDead = true;
	}
	
	
}

void ANetTPSCharacter::DieProcess()
{
	auto pc = Cast<APlayerController>(Controller);
	pc->SetShowMouseCursor(true);
	GetFollowCamera()->PostProcessSettings.ColorSaturation = FVector4(0.0f, 0.0f, 0.0f, 1.0f);
	
	// 게임오버 UI 표시
	mainUI->GameoverUI->SetVisibility(ESlateVisibility::Visible);
}

void ANetTPSCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	PrintNetLog();	
	
	// 머리 위 체력 바가 항상 카메라를 바라보도록 회전합니다.
	if ( hpUIComp && hpUIComp->GetVisibleFlag() )
	{
		FVector CamLoc = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();
		FVector Direction = CamLoc - hpUIComp->GetComponentLocation();
		Direction.Z = 0.0f;
		hpUIComp->SetWorldRotation(Direction.GetSafeNormal().ToOrientationRotator());		
	}
}

void ANetTPSCharacter::PrintNetLog()
{
	const FString conStr = GetNetConnection() != nullptr ? TEXT("Valid Connection") : TEXT("Invalid Connection");
		
	const FString logStr = FString::Printf(TEXT("Connection : %s\nLocalRole : %s\nRemoteRole : %s"), *conStr, *LOCALROLE, *REMOTEROLE);
	
	DrawDebugString( GetWorld(), GetActorLocation() + FVector::UpVector * 100.0f, logStr, nullptr, FColor::White, 0, true, 1);
	
}

void ANetTPSCharacter::PossessedBy(AController* NewController)
{
	PRINTLOG(TEXT("Begin"));
	
	Super::PossessedBy(NewController);
	
	ClientRPC_InitUIWidget();
	
	PRINTLOG(TEXT("End"));
	
}

void ANetTPSCharacter::StartVoiceChat()
{
	GetController<ANetPlayerController>()->StartTalking();
}

void ANetTPSCharacter::StopVoiceChat()
{
	GetController<ANetPlayerController>()->StopTalking();
}

void ANetTPSCharacter::MulticastRPC_SendMsg_Implementation(const FString& msg)
{
	auto pc = Cast<ANetPlayerController>(GetWorld()->GetFirstPlayerController());
	if ( pc )
	{
		if ( pc->mainUI )
		{
			pc->mainUI->ReceiveMsg(msg);
		}
	}
}

void ANetTPSCharacter::ServerRPC_SendMsg_Implementation(const FString& msg)
{
	MulticastRPC_SendMsg(msg);
}

void ANetTPSCharacter::ClientRPC_Reload_Implementation()
{
	if ( mainUI )
	{
		// 탄약 UI를 다시 구성합니다.
		mainUI->RemoveAllAmmo();
		
		for ( int i = 0; i < MaxBulletCount; i++ )
		{
			mainUI->AddBullet();
		}		
	}
	// 재장전 완료 상태로 전환합니다.
	IsReloading = false;
}

void ANetTPSCharacter::ServerRPC_Reload_Implementation()
{	
	// 탄약 수를 초기화합니다.
	BulletCount = MaxBulletCount;
	ClientRPC_Reload();
}

void ANetTPSCharacter::MulticastRPC_Fire_Implementation(bool bHit,
                                                        const FHitResult& hitInfo)
{
	if ( bHit )
	{
		// 맞은 부위에 파티클 표시
		if ( GunEffect )
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GunEffect, hitInfo.Location, FRotator(), true);
		}
				
		// 사격 애니메이션 재생
		auto anim = Cast<UNetPlayerAnimInstance>(GetMesh()->GetAnimInstance());
		if ( anim != nullptr )
		{
			anim->PlayFireAnimation();
		}
	}	
}

void ANetTPSCharacter::ServerRPC_Fire_Implementation()
{
	// 서버에서 카메라 방향으로 사격 판정을 수행합니다.
	FHitResult hitInfo;
	FVector startPos = FollowCamera->GetComponentLocation();
	FVector endPos = startPos + FollowCamera->GetForwardVector() * 10000.0f;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);	
	
	if ( bHit )
	{	
		// 맞은 대상이 플레이어면 데미지를 적용합니다.
		auto otherPlayer = Cast<ANetTPSCharacter>(hitInfo.GetActor());
		if ( otherPlayer )
		{
			otherPlayer->DamageProcess();
			
			// 공격자의 점수를 갱신합니다.
			auto ps = Cast<ANetPlayerState>(GetPlayerState());
			ps->SetScore( ps->GetScore() + 1 );
		}		
	}
	
	// 탄약을 1발 소모합니다.
	BulletCount--;
	OnRep_BulletCount();
	
	MulticastRPC_Fire(bHit, hitInfo);
}

void ANetTPSCharacter::MulticastRPC_ReleasePistol_Implementation(
	AActor* pistolActor)
{
	// 모든 클라이언트에서 권총을 분리합니다.
	DetachPistol(pistolActor);
}

void ANetTPSCharacter::ServerRPC_ReleasePistol_Implementation()
{
	// 서버 상태를 권총 미소유로 갱신합니다.
	if ( ownedPistol )
	{
		MulticastRPC_ReleasePistol(ownedPistol);
		
		bHasPistol = false;
		ownedPistol->SetOwner(nullptr);
		ownedPistol = nullptr;		
	}
}

void ANetTPSCharacter::MulticastRPC_TakePistol_Implementation(
	AActor* pistolActor)
{
	// 모든 클라이언트에서 권총을 장착합니다.
	AttachPistol(pistolActor);
}

void ANetTPSCharacter::ServerRPC_TakePistol_Implementation()
{	
	// 월드의 권총 중 소유자가 없고 가장 먼저 범위에 들어온 액터를 장착합니다.
	for ( auto pistolActor : pistolActors )
	{
		if ( pistolActor->GetOwner() != nullptr )
		{
			continue;
		}

		float Distance = FVector::Dist(GetActorLocation(), pistolActor->GetActorLocation());
		
		if ( Distance > DistanceToGun )
		{
			continue;
		}
		
		ownedPistol = pistolActor;
		
		ownedPistol->SetOwner(this);
		
		bHasPistol = true;
		
		MulticastRPC_TakePistol(pistolActor);		
		
		break;
	}
}


void ANetTPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ANetTPSCharacter,bHasPistol);
	DOREPLIFETIME(ANetTPSCharacter,BulletCount);
	DOREPLIFETIME(ANetTPSCharacter,hp);
	DOREPLIFETIME(ANetTPSCharacter,ownedPistol);
}



























































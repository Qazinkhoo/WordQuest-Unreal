#include "WordQuestCharacter.h"
#include "WordQuestGameInstance.h"
#include "WordQuestHUD.h"
#include "WordQuestBodyPartWidget.h"
#include "WordQuestShadowWidget.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

AWordQuestCharacter::AWordQuestCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->GravityScale = 2.f;
    GetCharacterMovement()->AirControl = 0.8f;
    GetCharacterMovement()->JumpZVelocity = 700.f;
    GetCharacterMovement()->MaxWalkSpeed = 500.f;
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.f, 1.f, 0.f));

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 900.f;
    CameraBoom->SetUsingAbsoluteRotation(true);
    CameraBoom->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    CameraBoom->bDoCollisionTest = false;

    SideCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("SideCamera"));
    SideCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    SideCamera->bUsePawnControlRotation = false;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

    auto MakeBlock = [this](const TCHAR* Name, const FVector& Scale, const FVector& Location)
    {
        UStaticMeshComponent* Part = CreateDefaultSubobject<UStaticMeshComponent>(Name);
        Part->SetupAttachment(GetCapsuleComponent());
        Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        if (CubeMesh.Succeeded()) Part->SetStaticMesh(CubeMesh.Object);
        Part->SetRelativeScale3D(Scale);
        Part->SetRelativeLocation(Location);
        Part->SetVisibility(false, true);
        return Part;
    };

    BlockBody = MakeBlock(TEXT("BlockBody"), FVector(0.45f, 0.28f, 0.62f), FVector(0.f, 0.f, 15.f));
    BlockHead = MakeBlock(TEXT("BlockHead"), FVector(0.36f, 0.32f, 0.36f), FVector(0.f, 0.f, 86.f));
    LeftArm = MakeBlock(TEXT("LeftArm"), FVector(0.16f, 0.16f, 0.55f), FVector(0.f, -43.f, 20.f));
    RightArm = MakeBlock(TEXT("RightArm"), FVector(0.16f, 0.16f, 0.55f), FVector(0.f, 43.f, 20.f));
    LeftLeg = MakeBlock(TEXT("LeftLeg"), FVector(0.18f, 0.18f, 0.55f), FVector(0.f, -18.f, -68.f));
    RightLeg = MakeBlock(TEXT("RightLeg"), FVector(0.18f, 0.18f, 0.55f), FVector(0.f, 18.f, -68.f));

    auto MakeHeroPart = [this](const TCHAR* Name, const FVector2D& DrawSize, const FVector2D& Pivot, const FVector& Location)
    {
        UWidgetComponent* Part = CreateDefaultSubobject<UWidgetComponent>(Name);
        Part->SetupAttachment(GetCapsuleComponent());
        Part->SetWidgetClass(UWordQuestBodyPartWidget::StaticClass());
        Part->SetWidgetSpace(EWidgetSpace::World);
        Part->SetDrawSize(DrawSize);
        Part->SetPivot(Pivot);
        Part->SetRelativeLocation(Location);
        Part->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
        Part->SetTwoSided(true);
        Part->SetBlendMode(EWidgetBlendMode::Transparent);
        Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        return Part;
    };

    HeadPartComponent = MakeHeroPart(TEXT("HeadPartComponent"), FVector2D(104.f, 88.f), FVector2D(0.50f, 0.50f), FVector(8.f, 0.f, 156.f));
    TorsoPartComponent = MakeHeroPart(TEXT("TorsoPartComponent"), FVector2D(92.f, 105.f), FVector2D(0.50f, 0.50f), FVector(0.f, -2.f, 60.f));
    LeftArmPartComponent = MakeHeroPart(TEXT("LeftArmPartComponent"), FVector2D(53.f, 115.f), FVector2D(0.50f, 0.08f), FVector(-25.f, -3.f, 105.f));
    RightArmPartComponent = MakeHeroPart(TEXT("RightArmPartComponent"), FVector2D(70.f, 115.f), FVector2D(0.50f, 0.08f), FVector(18.f, -1.f, 105.f));
    LeftLegPartComponent = MakeHeroPart(TEXT("LeftLegPartComponent"), FVector2D(67.f, 102.f), FVector2D(0.50f, 0.05f), FVector(-10.f, -3.f, 0.f));
    RightLegPartComponent = MakeHeroPart(TEXT("RightLegPartComponent"), FVector2D(72.f, 104.f), FVector2D(0.50f, 0.05f), FVector(16.f, -1.f, 0.f));

    ShadowWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ShadowWidgetComponent"));
    ShadowWidgetComponent->SetupAttachment(GetCapsuleComponent());
    ShadowWidgetComponent->SetWidgetClass(UWordQuestShadowWidget::StaticClass());
    ShadowWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
    ShadowWidgetComponent->SetDrawSize(FVector2D(92.f, 10.f));
    ShadowWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
    ShadowWidgetComponent->SetRelativeLocation(FVector(0.f, 1.f, -94.f));
    ShadowWidgetComponent->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
    ShadowWidgetComponent->SetTwoSided(true);
    ShadowWidgetComponent->SetBlendMode(EWidgetBlendMode::Transparent);
    ShadowWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWordQuestCharacter::BeginPlay()
{
    Super::BeginPlay();

    HeadPartWidget = Cast<UWordQuestBodyPartWidget>(HeadPartComponent ? HeadPartComponent->GetUserWidgetObject() : nullptr);
    TorsoPartWidget = Cast<UWordQuestBodyPartWidget>(TorsoPartComponent ? TorsoPartComponent->GetUserWidgetObject() : nullptr);
    LeftArmPartWidget = Cast<UWordQuestBodyPartWidget>(LeftArmPartComponent ? LeftArmPartComponent->GetUserWidgetObject() : nullptr);
    RightArmPartWidget = Cast<UWordQuestBodyPartWidget>(RightArmPartComponent ? RightArmPartComponent->GetUserWidgetObject() : nullptr);
    LeftLegPartWidget = Cast<UWordQuestBodyPartWidget>(LeftLegPartComponent ? LeftLegPartComponent->GetUserWidgetObject() : nullptr);
    RightLegPartWidget = Cast<UWordQuestBodyPartWidget>(RightLegPartComponent ? RightLegPartComponent->GetUserWidgetObject() : nullptr);

    if (HeadPartWidget) HeadPartWidget->SetBodyPart(EWordQuestBodyPart::Head);
    if (TorsoPartWidget) TorsoPartWidget->SetBodyPart(EWordQuestBodyPart::Torso);
    if (LeftArmPartWidget) LeftArmPartWidget->SetBodyPart(EWordQuestBodyPart::LeftArm);
    if (RightArmPartWidget) RightArmPartWidget->SetBodyPart(EWordQuestBodyPart::RightArm);
    if (LeftLegPartWidget) LeftLegPartWidget->SetBodyPart(EWordQuestBodyPart::LeftLeg);
    if (RightLegPartWidget) RightLegPartWidget->SetBodyPart(EWordQuestBodyPart::RightLeg);

    UpdateFacing();
    UpdateGroundShadow();
}

void AWordQuestCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    UpdateHeroVisual(DeltaSeconds);
    UpdateGroundShadow();
}

void AWordQuestCharacter::UpdateFacing()
{
    const bool bFlip = bFacingLeft;

    if (HeadPartWidget) HeadPartWidget->SetFlipped(bFlip);
    if (TorsoPartWidget) TorsoPartWidget->SetFlipped(bFlip);
    if (LeftArmPartWidget) LeftArmPartWidget->SetFlipped(bFlip);
    if (RightArmPartWidget) RightArmPartWidget->SetFlipped(bFlip);
    if (LeftLegPartWidget) LeftLegPartWidget->SetFlipped(bFlip);
    if (RightLegPartWidget) RightLegPartWidget->SetFlipped(bFlip);
}

void AWordQuestCharacter::UpdateHeroVisual(float DeltaSeconds)
{
    const float SpeedX = GetVelocity().X;
    const bool bMovingOnGround = FMath::Abs(SpeedX) > 5.f && GetCharacterMovement()->IsMovingOnGround();
    const bool bInAir = GetCharacterMovement()->IsFalling();

    if (FMath::Abs(SpeedX) > 1.f)
    {
        const bool bNewFacingLeft = SpeedX < 0.f;
        if (bNewFacingLeft != bFacingLeft)
        {
            bFacingLeft = bNewFacingLeft;
            UpdateFacing();
        }
    }

    MovementAnimTime += DeltaSeconds;

    const float FacingSign = bFacingLeft ? -1.f : 1.f;
    const float WalkPhase = MovementAnimTime * 8.5f;
    const float WalkSin = FMath::Sin(WalkPhase);
    const float WalkCos = FMath::Cos(WalkPhase);

    float ArmSwing = 0.f;
    float LegSwing = 0.f;
    float Bob = 0.f;
    float HeadBob = 0.f;

    if (bMovingOnGround)
    {
        ArmSwing = WalkSin * 24.f;
        LegSwing = WalkSin * 22.f;
        Bob = FMath::Abs(WalkSin) * 2.5f;
        HeadBob = FMath::Abs(WalkCos) * 1.5f;
    }
    else if (bInAir)
    {
        ArmSwing = -18.f;
        LegSwing = 14.f;
        Bob = 3.f;
        HeadBob = 2.f;
    }
    else
    {
        Bob = FMath::Sin(MovementAnimTime * 2.0f) * 0.8f;
        HeadBob = FMath::Sin(MovementAnimTime * 2.0f + 0.6f) * 0.6f;
    }

    const float BackArmX = -25.f * FacingSign;
    const float FrontArmX = 18.f * FacingSign;
    const float BackLegX = -10.f * FacingSign;
    const float FrontLegX = 16.f * FacingSign;
    const float HeadX = 8.f * FacingSign;

    if (HeadPartComponent)
    {
        HeadPartComponent->SetRelativeLocation(FVector(HeadX, 0.f, 156.f + Bob + HeadBob));
        HeadPartComponent->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
    }

    if (TorsoPartComponent)
    {
        TorsoPartComponent->SetRelativeLocation(FVector(0.f, -2.f, 60.f + Bob));
        TorsoPartComponent->SetRelativeRotation(FRotator(bMovingOnGround ? WalkSin * 1.5f : 0.f, 90.f, 0.f));
    }

    if (LeftArmPartComponent)
    {
        LeftArmPartComponent->SetRelativeLocation(FVector(BackArmX, -3.f, 105.f + Bob));
        LeftArmPartComponent->SetRelativeRotation(FRotator((-ArmSwing) * FacingSign, 90.f, 0.f));
    }

    if (RightArmPartComponent)
    {
        RightArmPartComponent->SetRelativeLocation(FVector(FrontArmX, -1.f, 105.f + Bob));
        RightArmPartComponent->SetRelativeRotation(FRotator(ArmSwing * FacingSign, 90.f, 0.f));
    }

    if (LeftLegPartComponent)
    {
        LeftLegPartComponent->SetRelativeLocation(FVector(BackLegX, -3.f, Bob * 0.35f));
        LeftLegPartComponent->SetRelativeRotation(FRotator(LegSwing * FacingSign, 90.f, 0.f));
    }

    if (RightLegPartComponent)
    {
        RightLegPartComponent->SetRelativeLocation(FVector(FrontLegX, -1.f, Bob * 0.35f));
        RightLegPartComponent->SetRelativeRotation(FRotator((-LegSwing) * FacingSign, 90.f, 0.f));
    }
}

void AWordQuestCharacter::UpdateGroundShadow()
{
    if (!ShadowWidgetComponent || !GetWorld()) return;

    const FVector ActorLocation = GetActorLocation();
    const FVector TraceStart = ActorLocation + FVector(0.f, 0.f, 20.f);
    const FVector TraceEnd = ActorLocation - FVector(0.f, 0.f, 650.f);

    FHitResult Hit;
    FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WordQuestPlayerShadow), false, this);
    const bool bHitGround = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams);

    if (!bHitGround)
    {
        ShadowWidgetComponent->SetVisibility(false);
        return;
    }

    ShadowWidgetComponent->SetVisibility(true);
    ShadowWidgetComponent->SetWorldLocation(Hit.ImpactPoint + FVector(0.f, 1.f, 2.5f));

    const float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    const float FeetHeight = ActorLocation.Z - CapsuleHalfHeight;
    const float HeightAboveGround = FMath::Max(0.f, FeetHeight - Hit.ImpactPoint.Z);
    const float ShadowScale = FMath::Clamp(1.f - HeightAboveGround / 320.f, 0.42f, 1.f);

    ShadowWidgetComponent->SetDrawSize(FVector2D(92.f * ShadowScale, FMath::Max(4.f, 10.f * ShadowScale)));
}

void AWordQuestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AWordQuestCharacter::MoveRight);
    PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ACharacter::StopJumping);
}

void AWordQuestCharacter::MoveRight(float Value)
{
    if (bBattleLocked) return;
    if (!FMath::IsNearlyZero(Value)) AddMovementInput(FVector(1.f, 0.f, 0.f), Value);
}

void AWordQuestCharacter::CollectSword()
{
    if (UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>()) GI->CollectSword();
}

void AWordQuestCharacter::SetBattleLocked(bool bLocked)
{
    bBattleLocked = bLocked;
    if (bBattleLocked)
    {
        GetCharacterMovement()->StopMovementImmediately();
    }
}

void AWordQuestCharacter::ShowFloatingText(const FString& InText, const FColor& InColor, float HeightOffset)
{
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC && GetWorld()) PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;

    if (AWordQuestHUD* HUD = Cast<AWordQuestHUD>(PC->GetHUD()))
    {
        HUD->AddFloatingMessage(InText, InColor, GetActorLocation() + FVector(0.f, 0.f, HeightOffset));
    }
}

void AWordQuestCharacter::PlayDamageCameraBump()
{
    if (!SideCamera || !GetWorld()) return;

    const FVector OriginalLocation = SideCamera->GetRelativeLocation();
    SideCamera->SetRelativeLocation(OriginalLocation + FVector(0.f, 0.f, 18.f));

    FTimerHandle ResetHandle;
    TWeakObjectPtr<UCameraComponent> WeakCamera = SideCamera;
    GetWorld()->GetTimerManager().SetTimer(ResetHandle, [WeakCamera, OriginalLocation]()
    {
        if (WeakCamera.IsValid())
        {
            WeakCamera->SetRelativeLocation(OriginalLocation);
        }
    }, 0.10f, false);
}

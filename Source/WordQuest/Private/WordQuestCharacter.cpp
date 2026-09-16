#include "WordQuestCharacter.h"
#include "WordQuestGameInstance.h"
#include "WordQuestHUD.h"
#include "WordQuestHeroWidget.h"
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

    // Use the original coherent side-view hero. A 100 x 300 draw size matches
    // the source character's tall 1:3 proportions and keeps the feet grounded.
    HeroWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HeroWidgetComponent"));
    HeroWidgetComponent->SetupAttachment(GetCapsuleComponent());
    HeroWidgetComponent->SetWidgetClass(UWordQuestHeroWidget::StaticClass());
    HeroWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
    HeroWidgetComponent->SetDrawSize(FVector2D(100.f, 300.f));
    HeroWidgetComponent->SetPivot(FVector2D(0.5f, 1.f));
    HeroWidgetComponent->SetRelativeLocation(FVector(0.f, -2.f, -94.f));
    HeroWidgetComponent->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
    HeroWidgetComponent->SetTwoSided(true);
    HeroWidgetComponent->SetBlendMode(EWidgetBlendMode::Transparent);
    HeroWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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

    if (HeroWidgetComponent)
    {
        HeroWidget = Cast<UWordQuestHeroWidget>(HeroWidgetComponent->GetUserWidgetObject());
        if (HeroWidget)
        {
            HeroWidget->SetFacingLeft(false);
            HeroWidget->SetMovementState(false, false);
        }
    }

    UpdateGroundShadow();
}

void AWordQuestCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    UpdateHeroVisual(DeltaSeconds);
    UpdateGroundShadow();
}

void AWordQuestCharacter::UpdateHeroVisual(float DeltaSeconds)
{
    if (!HeroWidget) return;

    const float SpeedX = GetVelocity().X;
    const bool bMovingOnGround = FMath::Abs(SpeedX) > 5.f && GetCharacterMovement()->IsMovingOnGround();
    const bool bInAir = GetCharacterMovement()->IsFalling();

    if (FMath::Abs(SpeedX) > 1.f)
    {
        const bool bNewFacingLeft = SpeedX < 0.f;
        if (bNewFacingLeft != bFacingLeft)
        {
            bFacingLeft = bNewFacingLeft;
            HeroWidget->SetFacingLeft(bFacingLeft);
        }
    }

    HeroWidget->SetMovementState(bMovingOnGround, bInAir);
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
    if (!FMath::IsNearlyZero(Value))
    {
        AddMovementInput(FVector(1.f, 0.f, 0.f), Value);
    }
}

void AWordQuestCharacter::CollectSword()
{
    if (UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>())
    {
        GI->CollectSword();
    }
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

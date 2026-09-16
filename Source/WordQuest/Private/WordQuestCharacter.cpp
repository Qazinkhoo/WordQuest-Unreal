#include "WordQuestCharacter.h"
#include "WordQuestGameInstance.h"
#include "WordQuestFloatingText.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

AWordQuestCharacter::AWordQuestCharacter()
{
    PrimaryActorTick.bCanEverTick = false;
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
        return Part;
    };

    BlockBody = MakeBlock(TEXT("BlockBody"), FVector(0.45f, 0.28f, 0.62f), FVector(0.f, 0.f, 15.f));
    BlockHead = MakeBlock(TEXT("BlockHead"), FVector(0.36f, 0.32f, 0.36f), FVector(0.f, 0.f, 86.f));
    LeftArm = MakeBlock(TEXT("LeftArm"), FVector(0.16f, 0.16f, 0.55f), FVector(0.f, -43.f, 20.f));
    RightArm = MakeBlock(TEXT("RightArm"), FVector(0.16f, 0.16f, 0.55f), FVector(0.f, 43.f, 20.f));
    LeftLeg = MakeBlock(TEXT("LeftLeg"), FVector(0.18f, 0.18f, 0.55f), FVector(0.f, -18.f, -68.f));
    RightLeg = MakeBlock(TEXT("RightLeg"), FVector(0.18f, 0.18f, 0.55f), FVector(0.f, 18.f, -68.f));
}

void AWordQuestCharacter::BeginPlay()
{
    Super::BeginPlay();
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
    UWorld* World = GetWorld();
    if (!World) return;

    const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, HeightOffset);
    AWordQuestFloatingText* FloatingText = World->SpawnActor<AWordQuestFloatingText>(SpawnLocation, FRotator(0.f, 90.f, 0.f));
    if (FloatingText)
    {
        FloatingText->SetupText(InText, InColor);
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

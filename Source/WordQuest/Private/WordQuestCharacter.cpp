#include "WordQuestCharacter.h"
#include "WordQuestGameInstance.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"

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
    if (!FMath::IsNearlyZero(Value)) AddMovementInput(FVector(1.f, 0.f, 0.f), Value);
}

void AWordQuestCharacter::CollectSword()
{
    if (UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>()) GI->CollectSword();
}

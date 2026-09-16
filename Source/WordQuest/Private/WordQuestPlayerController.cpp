#include "WordQuestPlayerController.h"
#include "WordQuestGameMode.h"
#include "InputCoreTypes.h"
#include "Engine/World.h"
#include "Engine/UserInterfaceSettings.h"

void AWordQuestPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Disable Unreal's keyboard/gamepad focus brush at runtime so the intro
    // screen shows only the baked artwork without the yellow selection box.
    if (UUserInterfaceSettings* UISettings = GetMutableDefault<UUserInterfaceSettings>())
    {
        UISettings->RenderFocusRule = ERenderFocusRule::Never;
    }

    if (!InputComponent) return;

    InputComponent->BindKey(EKeys::One, IE_Pressed, this, &AWordQuestPlayerController::PressOption1);
    InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &AWordQuestPlayerController::PressOption2);
    InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &AWordQuestPlayerController::PressOption3);
    InputComponent->BindKey(EKeys::Four, IE_Pressed, this, &AWordQuestPlayerController::PressOption4);
    InputComponent->BindKey(EKeys::Enter, IE_Pressed, this, &AWordQuestPlayerController::PressEnter);
}

void AWordQuestPlayerController::PressOption1()
{
    if (!GetWorld()) return;
    if (AWordQuestGameMode* GM = GetWorld()->GetAuthGameMode<AWordQuestGameMode>())
    {
        if (GM->bMainMenuOpen || GM->bQuestComplete) return;
        if (GM->bGameOver) GM->RestartCurrentStage();
        else if (GM->bShopOpen) GM->BuyShopApple();
        else SubmitAnswerIndex(0);
    }
}

void AWordQuestPlayerController::PressOption2()
{
    if (!GetWorld()) return;
    if (AWordQuestGameMode* GM = GetWorld()->GetAuthGameMode<AWordQuestGameMode>())
    {
        if (GM->bMainMenuOpen || GM->bQuestComplete) return;
        if (GM->bGameOver) GM->ReturnToMainMenu();
        else if (GM->bShopOpen) GM->BuyShopStar();
        else SubmitAnswerIndex(1);
    }
}

void AWordQuestPlayerController::PressOption3()
{
    if (!GetWorld()) return;
    if (AWordQuestGameMode* GM = GetWorld()->GetAuthGameMode<AWordQuestGameMode>())
    {
        if (GM->bMainMenuOpen || GM->bGameOver || GM->bQuestComplete) return;
        if (GM->bShopOpen) GM->BuyShopArmour();
        else SubmitAnswerIndex(2);
    }
}

void AWordQuestPlayerController::PressOption4()
{
    if (!GetWorld()) return;
    if (AWordQuestGameMode* GM = GetWorld()->GetAuthGameMode<AWordQuestGameMode>())
    {
        if (GM->bMainMenuOpen || GM->bGameOver || GM->bQuestComplete) return;
        if (GM->bShopOpen) GM->LeaveStageShop();
        else SubmitAnswerIndex(3);
    }
}

void AWordQuestPlayerController::PressEnter()
{
    if (!GetWorld()) return;
    if (AWordQuestGameMode* GM = GetWorld()->GetAuthGameMode<AWordQuestGameMode>())
    {
        if (GM->bMainMenuOpen) GM->StartAdventureFromMenu();
        else if (GM->bQuestComplete) GM->ReturnToMainMenu();
        else if (GM->bShopOpen) GM->LeaveStageShop();
    }
}

void AWordQuestPlayerController::SubmitAnswerIndex(int32 AnswerIndex)
{
    if (!GetWorld()) return;

    if (AWordQuestGameMode* GM = GetWorld()->GetAuthGameMode<AWordQuestGameMode>())
    {
        if (!GM->bMainMenuOpen && !GM->bGameOver && !GM->bQuestComplete)
        {
            GM->SubmitAnswer(AnswerIndex);
        }
    }
}

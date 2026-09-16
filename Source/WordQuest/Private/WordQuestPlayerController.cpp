#include "WordQuestPlayerController.h"
#include "WordQuestGameMode.h"
#include "InputCoreTypes.h"
#include "Engine/World.h"

void AWordQuestPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (!InputComponent) return;

    InputComponent->BindKey(EKeys::One, IE_Pressed, this, &AWordQuestPlayerController::SubmitAnswer1);
    InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &AWordQuestPlayerController::SubmitAnswer2);
    InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &AWordQuestPlayerController::SubmitAnswer3);
    InputComponent->BindKey(EKeys::Four, IE_Pressed, this, &AWordQuestPlayerController::SubmitAnswer4);
}

void AWordQuestPlayerController::SubmitAnswer1() { SubmitAnswerIndex(0); }
void AWordQuestPlayerController::SubmitAnswer2() { SubmitAnswerIndex(1); }
void AWordQuestPlayerController::SubmitAnswer3() { SubmitAnswerIndex(2); }
void AWordQuestPlayerController::SubmitAnswer4() { SubmitAnswerIndex(3); }

void AWordQuestPlayerController::SubmitAnswerIndex(int32 AnswerIndex)
{
    if (!GetWorld()) return;

    if (AWordQuestGameMode* GM = GetWorld()->GetAuthGameMode<AWordQuestGameMode>())
    {
        GM->SubmitAnswer(AnswerIndex);
    }
}

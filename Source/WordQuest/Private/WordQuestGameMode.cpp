#include "WordQuestGameMode.h"
#include "WordQuestCharacter.h"
#include "WordQuestEnemy.h"
#include "WordQuestGameInstance.h"
#include "WordQuestQuestionSubsystem.h"
#include "WordQuestStageOneBuilder.h"
#include "Kismet/GameplayStatics.h"

AWordQuestGameMode::AWordQuestGameMode()
{
    DefaultPawnClass = AWordQuestCharacter::StaticClass();
}

void AWordQuestGameMode::BeginPlay()
{
    Super::BeginPlay();
    CurrentWave = 1;
    bBattleActive = false;
    bStageClear = false;
    if (UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>())
    {
        if (GI->PlayerState.Stage <= 1 && GI->PlayerState.CurrentHP <= 0) GI->StartNewAdventure();
        GI->PlayerState.Stage = 1;
        GI->PlayerState.Wave = 1;
    }

    if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0))
    {
        const FVector BuilderLocation = Pawn->GetActorLocation() - FVector(150.f, 0.f, Pawn->GetActorLocation().Z);
        GetWorld()->SpawnActor<AWordQuestStageOneBuilder>(BuilderLocation, FRotator::ZeroRotator);
    }
}

void AWordQuestGameMode::StartEncounter(AWordQuestEnemy* Enemy)
{
    if (!Enemy || bBattleActive || bStageClear) return;
    CurrentEnemy = Enemy;
    CurrentWave = Enemy->WaveNumber;
    bBattleActive = true;

    if (AWordQuestCharacter* Player = Cast<AWordQuestCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        Player->SetBattleLocked(true);
    }

    LoadDifferentQuestion();
    OnBattleStateChanged();
}

bool AWordQuestGameMode::LoadDifferentQuestion()
{
    if (UWordQuestQuestionSubsystem* Questions = GetGameInstance()->GetSubsystem<UWordQuestQuestionSubsystem>())
    {
        if (Questions->GetNextQuestion(CurrentQuestion))
        {
            OnQuestionChanged();
            return true;
        }
    }
    return false;
}

bool AWordQuestGameMode::SubmitAnswer(int32 AnswerIndex)
{
    if (!bBattleActive || !CurrentEnemy) return false;
    UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>();
    if (!GI) return false;

    const bool bCorrect = AnswerIndex == CurrentQuestion.CorrectAnswerIndex;
    if (bCorrect)
    {
        const bool bDefeated = CurrentEnemy->ReceiveWordDamage(GI->PlayerState.Damage);
        if (bDefeated)
        {
            if (CurrentEnemy->bBoss) GI->RewardBoss(); else GI->RewardNormalEnemy();
            CurrentEnemy->Destroy();
            CurrentEnemy = nullptr;
            bBattleActive = false;
            AdvanceWave();

            if (AWordQuestCharacter* Player = Cast<AWordQuestCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
            {
                Player->SetBattleLocked(false);
            }

            OnBattleStateChanged();
            return true;
        }
    }
    else
    {
        GI->ApplyEnemyHit();
        if (GI->IsGameOver())
        {
            bBattleActive = false;
            if (AWordQuestCharacter* Player = Cast<AWordQuestCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
            {
                Player->SetBattleLocked(true);
            }
            OnGameOver();
            OnBattleStateChanged();
            return false;
        }
    }
    LoadDifferentQuestion();
    return bCorrect;
}

void AWordQuestGameMode::AdvanceWave()
{
    if (CurrentWave >= 5)
    {
        bStageClear = true;
        OnStageCleared();
        return;
    }
    ++CurrentWave;
    if (UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>()) GI->PlayerState.Wave = CurrentWave;
}

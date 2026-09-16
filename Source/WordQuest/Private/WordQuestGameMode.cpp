#include "WordQuestGameMode.h"
#include "WordQuestCharacter.h"
#include "WordQuestEnemy.h"
#include "WordQuestGameInstance.h"
#include "WordQuestQuestionSubsystem.h"
#include "WordQuestStageOneBuilder.h"
#include "WordQuestHUD.h"
#include "WordQuestPlayerController.h"
#include "Kismet/GameplayStatics.h"

AWordQuestGameMode::AWordQuestGameMode()
{
    DefaultPawnClass = AWordQuestCharacter::StaticClass();
    HUDClass = AWordQuestHUD::StaticClass();
    PlayerControllerClass = AWordQuestPlayerController::StaticClass();
}

void AWordQuestGameMode::BeginPlay()
{
    Super::BeginPlay();
    CurrentWave = 1;
    bBattleActive = false;
    bStageClear = false;
    bShopOpen = false;

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
    if (!Enemy || bBattleActive || bStageClear || bShopOpen) return;
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

    AWordQuestCharacter* Player = Cast<AWordQuestCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    const bool bCorrect = AnswerIndex == CurrentQuestion.CorrectAnswerIndex;

    if (bCorrect)
    {
        if (Player)
        {
            Player->ShowFloatingText(TEXT("Correct!"), FColor::White, 195.f);
        }

        CurrentEnemy->PlayHitPulse();
        const bool bWasBoss = CurrentEnemy->bBoss;
        const bool bDefeated = CurrentEnemy->ReceiveWordDamage(GI->PlayerState.Damage);

        if (bDefeated)
        {
            if (bWasBoss)
            {
                GI->RewardBoss();
                if (Player) Player->ShowFloatingText(TEXT("+10 Coins"), FColor::Yellow, 150.f);
            }
            else
            {
                GI->RewardNormalEnemy();
                if (Player) Player->ShowFloatingText(TEXT("+3 Coins"), FColor::Yellow, 150.f);
            }

            CurrentEnemy->Destroy();
            CurrentEnemy = nullptr;
            bBattleActive = false;
            AdvanceWave();

            if (Player)
            {
                Player->SetBattleLocked(bStageClear || bShopOpen);
            }

            OnBattleStateChanged();
            return true;
        }
    }
    else
    {
        const bool bTookDamage = GI->ApplyEnemyHit();

        if (Player)
        {
            if (bTookDamage)
            {
                Player->ShowFloatingText(TEXT("-1 HP"), FColor::Red, 150.f);
                Player->ShowFloatingText(TEXT("Wrong!"), FColor::Red, 205.f);
                Player->PlayDamageCameraBump();
            }
            else
            {
                Player->ShowFloatingText(TEXT("Blocked!"), FColor::Cyan, 175.f);
            }
        }

        if (GI->IsGameOver())
        {
            bBattleActive = false;
            if (Player) Player->SetBattleLocked(true);
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
        OpenStageShop();
        OnStageCleared();
        return;
    }

    ++CurrentWave;
    if (UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>())
    {
        GI->PlayerState.Wave = CurrentWave;
    }
}

void AWordQuestGameMode::OpenStageShop()
{
    bShopOpen = true;
    if (UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>())
    {
        GI->ResetShopStock();
    }

    if (AWordQuestCharacter* Player = Cast<AWordQuestCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        Player->SetBattleLocked(true);
    }
}

void AWordQuestGameMode::BuyShopApple()
{
    if (!bShopOpen) return;
    if (UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>()) GI->BuyApple();
}

void AWordQuestGameMode::BuyShopStar()
{
    if (!bShopOpen) return;
    if (UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>()) GI->BuyStar();
}

void AWordQuestGameMode::BuyShopArmour()
{
    if (!bShopOpen) return;
    if (UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>()) GI->BuyArmour();
}

void AWordQuestGameMode::LeaveStageShop()
{
    if (!bShopOpen) return;
    bShopOpen = false;

    if (AWordQuestCharacter* Player = Cast<AWordQuestCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        Player->ShowFloatingText(TEXT("Stage 2 coming next"), FColor::White, 180.f);
        Player->SetBattleLocked(true);
    }
}

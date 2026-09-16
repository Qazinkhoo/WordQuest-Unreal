#include "WordQuestGameMode.h"
#include "WordQuestCharacter.h"
#include "WordQuestEnemy.h"
#include "WordQuestGameInstance.h"
#include "WordQuestQuestionSubsystem.h"
#include "WordQuestStageOneBuilder.h"
#include "WordQuestStageTwoBuilder.h"
#include "WordQuestStageThreeBuilder.h"
#include "WordQuestHUD.h"
#include "WordQuestPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWaveProcedural.h"

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

    UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>();
    if (!GI) return;

    const FString LevelName = UGameplayStatics::GetCurrentLevelName(this, true);
    const bool bIsStageThreeMap = LevelName.Contains(TEXT("Stage03_CrystalCave"));
    const bool bIsStageTwoMap = LevelName.Contains(TEXT("Stage02_SunnyMeadow"));

    if (bIsStageThreeMap)
    {
        CurrentStage = 3;
        GI->PlayerState.Stage = 3;
    }
    else if (bIsStageTwoMap)
    {
        CurrentStage = 2;
        GI->PlayerState.Stage = 2;
    }
    else
    {
        CurrentStage = 1;
        if (GI->PlayerState.CurrentHP <= 0) GI->StartNewAdventure();
        GI->PlayerState.Stage = 1;
    }

    GI->PlayerState.Wave = 1;
    CurrentWave = 1;

    if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0))
    {
        AdventureStartLocation = Pawn->GetActorLocation();
        const FVector BuilderLocation = Pawn->GetActorLocation() - FVector(150.f, 0.f, Pawn->GetActorLocation().Z);

        if (CurrentStage == 3)
        {
            GetWorld()->SpawnActor<AWordQuestStageThreeBuilder>(BuilderLocation, FRotator::ZeroRotator);
        }
        else if (CurrentStage == 2)
        {
            GetWorld()->SpawnActor<AWordQuestStageTwoBuilder>(BuilderLocation, FRotator::ZeroRotator);
        }
        else
        {
            GetWorld()->SpawnActor<AWordQuestStageOneBuilder>(BuilderLocation, FRotator::ZeroRotator);
        }
    }
}

void AWordQuestGameMode::StartEncounter(AWordQuestEnemy* Enemy)
{
    if (!Enemy || bBattleActive || bStageClear || bShopOpen) return;
    if (Enemy->WaveNumber != CurrentWave) return;

    CurrentEnemy = Enemy;
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

void AWordQuestGameMode::PlayTone(float StartFrequency, float EndFrequency, float DurationSeconds, float Volume)
{
    if (!GetWorld() || DurationSeconds <= 0.f) return;

    constexpr int32 SampleRate = 44100;
    const int32 NumSamples = FMath::Max(1, FMath::RoundToInt(DurationSeconds * SampleRate));

    USoundWaveProcedural* Sound = NewObject<USoundWaveProcedural>(this);
    if (!Sound) return;

    Sound->NumChannels = 1;
    Sound->Duration = DurationSeconds;
    Sound->bLooping = false;
    Sound->SetSampleRate(SampleRate);

    TArray<int16> PCM;
    PCM.SetNumUninitialized(NumSamples);

    double Phase = 0.0;
    for (int32 i = 0; i < NumSamples; ++i)
    {
        const float T = static_cast<float>(i) / static_cast<float>(FMath::Max(1, NumSamples - 1));
        const float Frequency = FMath::Lerp(StartFrequency, EndFrequency, T);
        Phase += 2.0 * PI * static_cast<double>(Frequency) / static_cast<double>(SampleRate);

        const float Attack = FMath::Clamp(T / 0.08f, 0.f, 1.f);
        const float Release = FMath::Clamp((1.f - T) / 0.15f, 0.f, 1.f);
        const float Envelope = FMath::Min(Attack, Release);
        const float Sample = FMath::Sin(static_cast<float>(Phase)) * Envelope * FMath::Clamp(Volume, 0.f, 1.f);
        PCM[i] = static_cast<int16>(FMath::Clamp(Sample, -1.f, 1.f) * 32767.f);
    }

    Sound->QueueAudio(reinterpret_cast<const uint8*>(PCM.GetData()), PCM.Num() * sizeof(int16));
    ActiveFeedbackSounds.Add(Sound);
    if (ActiveFeedbackSounds.Num() > 12) ActiveFeedbackSounds.RemoveAt(0);

    UGameplayStatics::PlaySound2D(this, Sound, 1.f, 1.f, 0.f);
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
        PlayTone(620.f, 880.f, 0.16f, 0.35f);
        if (Player) Player->ShowFloatingText(TEXT("Correct!"), FColor::White, 195.f);

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

            if (Player) Player->SetBattleLocked(bStageClear || bShopOpen);
            OnBattleStateChanged();
            return true;
        }
    }
    else
    {
        PlayTone(360.f, 180.f, 0.22f, 0.38f);
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
        PlayTone(440.f, 1040.f, 0.75f, 0.45f);
        OpenStageShop();
        OnStageCleared();
        return;
    }

    ++CurrentWave;
    if (UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>()) GI->PlayerState.Wave = CurrentWave;
}

void AWordQuestGameMode::OpenStageShop()
{
    bShopOpen = true;
    if (UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>()) GI->ResetShopStock();

    if (AWordQuestCharacter* Player = Cast<AWordQuestCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0))) Player->SetBattleLocked(true);
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

    if (CurrentStage == 1)
    {
        StartStageTwo();
        return;
    }

    if (CurrentStage == 2)
    {
        StartStageThree();
        return;
    }

    bShopOpen = false;
    if (AWordQuestCharacter* Player = Cast<AWordQuestCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0))) Player->SetBattleLocked(true);
}

void AWordQuestGameMode::StartStageTwo()
{
    UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>();
    if (!GI) return;

    GI->PlayerState.Stage = 2;
    GI->PlayerState.Wave = 1;
    bShopOpen = false;
    bBattleActive = false;
    bStageClear = false;
    CurrentEnemy = nullptr;

    UGameplayStatics::OpenLevel(this, FName(TEXT("Stage02_SunnyMeadow")));
}

void AWordQuestGameMode::StartStageThree()
{
    UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>();
    if (!GI) return;

    GI->PlayerState.Stage = 3;
    GI->PlayerState.Wave = 1;
    bShopOpen = false;
    bBattleActive = false;
    bStageClear = false;
    CurrentEnemy = nullptr;

    UGameplayStatics::OpenLevel(this, FName(TEXT("Stage03_CrystalCave")));
}

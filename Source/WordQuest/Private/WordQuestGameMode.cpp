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
#include "Kismet/KismetSystemLibrary.h"
#include "Sound/SoundWaveProcedural.h"
#include "TimerManager.h"

AWordQuestGameMode::AWordQuestGameMode()
{
    DefaultPawnClass = AWordQuestCharacter::StaticClass();
    HUDClass = AWordQuestHUD::StaticClass();
    PlayerControllerClass = AWordQuestPlayerController::StaticClass();
}

FName AWordQuestGameMode::GetBaseMapForStage(int32 Stage) const
{
    if (Stage <= 1) return FName(TEXT("Stage01_WhisperingForest"));
    return FName(TEXT("Stage02_SunnyMeadow"));
}

void AWordQuestGameMode::BeginPlay()
{
    Super::BeginPlay();

    CurrentWave = 1;
    bBattleActive = false;
    bStageClear = false;
    bShopOpen = false;
    bGameOver = false;
    bQuestComplete = false;
    bMainMenuOpen = false;
    bAnswerFeedbackActive = false;
    SelectedAnswerIndex = INDEX_NONE;
    MainMenuSelection = 0;

    UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>();
    if (!GI) return;

    const FString LevelName = UGameplayStatics::GetCurrentLevelName(this, true);
    const bool bIsStageThreeMap = LevelName.Contains(TEXT("Stage03_CrystalCave"));
    const bool bIsStageTwoMap = LevelName.Contains(TEXT("Stage02_SunnyMeadow"));

    if (GI->PlayerState.Stage == 3 || bIsStageThreeMap)
    {
        CurrentStage = 3;
        GI->PlayerState.Stage = 3;
    }
    else if (GI->PlayerState.Stage == 2 || bIsStageTwoMap)
    {
        CurrentStage = 2;
        GI->PlayerState.Stage = 2;
    }
    else
    {
        CurrentStage = 1;
        GI->PlayerState.Stage = 1;
    }

    GI->PlayerState.Wave = 1;
    CurrentWave = 1;

    if (CurrentStage == 1 && GI->ConsumeShowMainMenuOnStageOneLoad())
    {
        bMainMenuOpen = true;
        MainMenuSelection = 0;
        if (AWordQuestCharacter* Player = Cast<AWordQuestCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
        {
            Player->SetBattleLocked(true);
        }
        return;
    }

    if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0))
    {
        AdventureStartLocation = Pawn->GetActorLocation();
        const FVector BuilderLocation = Pawn->GetActorLocation() - FVector(150.f, 0.f, Pawn->GetActorLocation().Z);

        if (CurrentStage == 3) GetWorld()->SpawnActor<AWordQuestStageThreeBuilder>(BuilderLocation, FRotator::ZeroRotator);
        else if (CurrentStage == 2) GetWorld()->SpawnActor<AWordQuestStageTwoBuilder>(BuilderLocation, FRotator::ZeroRotator);
        else GetWorld()->SpawnActor<AWordQuestStageOneBuilder>(BuilderLocation, FRotator::ZeroRotator);
    }
}

void AWordQuestGameMode::MoveMainMenuSelection(int32 Direction)
{
    if (!bMainMenuOpen || Direction == 0) return;
    MainMenuSelection = (MainMenuSelection == 0) ? 1 : 0;
    PlayTone(420.f, 520.f, 0.08f, 0.18f);
}

void AWordQuestGameMode::ActivateMainMenuSelection()
{
    if (!bMainMenuOpen) return;

    if (MainMenuSelection == 0)
    {
        StartAdventureFromMenu();
    }
    else
    {
        ExitGame();
    }
}

void AWordQuestGameMode::ExitGame()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
}

void AWordQuestGameMode::StartAdventureFromMenu()
{
    if (!bMainMenuOpen) return;

    if (UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>())
    {
        GI->StartNewAdventure();
        GI->SaveStageCheckpoint();
        GI->SetShowMainMenuOnStageOneLoad(false);
    }

    UGameplayStatics::OpenLevel(this, FName(TEXT("Stage01_WhisperingForest")));
}

void AWordQuestGameMode::RestartCurrentStage()
{
    if (!bGameOver) return;

    if (UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>())
    {
        const int32 StageToRestart = GI->PlayerState.Stage;
        if (!GI->RestoreStageCheckpoint()) GI->StartNewAdventure();
        GI->SetShowMainMenuOnStageOneLoad(false);
        UGameplayStatics::OpenLevel(this, GetBaseMapForStage(StageToRestart));
    }
}

void AWordQuestGameMode::ReturnToMainMenu()
{
    if (UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>())
    {
        GI->StartNewAdventure();
        GI->SetShowMainMenuOnStageOneLoad(true);
    }

    UGameplayStatics::OpenLevel(this, FName(TEXT("Stage01_WhisperingForest")));
}

void AWordQuestGameMode::StartEncounter(AWordQuestEnemy* Enemy)
{
    if (!Enemy || bMainMenuOpen || bGameOver || bQuestComplete || bBattleActive || bStageClear || bShopOpen) return;
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
        if (Questions->GetNextQuestionForWave(CurrentWave, CurrentQuestion))
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
    if (bMainMenuOpen || bGameOver || bQuestComplete || bAnswerFeedbackActive || !bBattleActive || !CurrentEnemy) return false;
    if (!CurrentQuestion.Answers.IsValidIndex(AnswerIndex)) return false;

    SelectedAnswerIndex = AnswerIndex;
    bSelectedAnswerCorrect = AnswerIndex == CurrentQuestion.CorrectAnswerIndex;
    bAnswerFeedbackActive = true;
    AnswerFeedbackStartTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;

    FTimerHandle FeedbackTimer;
    GetWorldTimerManager().SetTimer(FeedbackTimer, this, &AWordQuestGameMode::ResolveSelectedAnswer, 1.0f, false);
    return bSelectedAnswerCorrect;
}

void AWordQuestGameMode::ResolveSelectedAnswer()
{
    if (!bAnswerFeedbackActive || !CurrentEnemy) return;

    UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>();
    if (!GI) return;

    AWordQuestCharacter* Player = Cast<AWordQuestCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    const bool bCorrect = bSelectedAnswerCorrect;

    if (bCorrect)
    {
        PlayTone(620.f, 880.f, 0.16f, 0.35f);
        if (Player) Player->ShowFloatingText(TEXT("Correct!"), FColor::Green, 195.f);

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
            bAnswerFeedbackActive = false;
            SelectedAnswerIndex = INDEX_NONE;
            AdvanceWave();

            if (Player) Player->SetBattleLocked(bStageClear || bShopOpen || bGameOver || bQuestComplete);
            OnBattleStateChanged();
            return;
        }

        bAnswerFeedbackActive = false;
        SelectedAnswerIndex = INDEX_NONE;
        LoadDifferentQuestion();
        return;
    }

    PlayTone(360.f, 180.f, 0.22f, 0.38f);
    const bool bTookDamage = GI->ApplyEnemyHit();

    if (Player)
    {
        if (bTookDamage)
        {
            Player->ShowFloatingText(TEXT("-1 HP"), FColor::Red, 150.f);
            Player->ShowFloatingText(TEXT("Wrong! Try again"), FColor::Red, 205.f);
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
        bGameOver = true;
        bAnswerFeedbackActive = false;
        SelectedAnswerIndex = INDEX_NONE;
        PlayTone(300.f, 90.f, 0.8f, 0.45f);
        if (Player) Player->SetBattleLocked(true);
        OnGameOver();
        OnBattleStateChanged();
        return;
    }

    bAnswerFeedbackActive = false;
    SelectedAnswerIndex = INDEX_NONE;
}

void AWordQuestGameMode::AdvanceWave()
{
    if (CurrentWave >= 5)
    {
        bStageClear = true;

        if (CurrentStage >= 3)
        {
            bQuestComplete = true;
            bShopOpen = false;
            bBattleActive = false;
            PlayTone(520.f, 1320.f, 1.15f, 0.5f);
            if (AWordQuestCharacter* Player = Cast<AWordQuestCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
            {
                Player->SetBattleLocked(true);
            }
            OnStageCleared();
            return;
        }

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
    if (CurrentStage >= 3) return;

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
    if (CurrentStage == 1) { StartStageTwo(); return; }
    if (CurrentStage == 2) { StartStageThree(); return; }
    bShopOpen = false;
}

void AWordQuestGameMode::StartStageTwo()
{
    UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>();
    if (!GI) return;

    GI->PlayerState.Stage = 2;
    GI->PlayerState.Wave = 1;
    GI->SaveStageCheckpoint();
    GI->SetShowMainMenuOnStageOneLoad(false);
    bShopOpen = false;
    bBattleActive = false;
    bStageClear = false;
    bGameOver = false;
    bQuestComplete = false;
    CurrentEnemy = nullptr;
    UGameplayStatics::OpenLevel(this, FName(TEXT("Stage02_SunnyMeadow")));
}

void AWordQuestGameMode::StartStageThree()
{
    UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>();
    if (!GI) return;

    GI->PlayerState.Stage = 3;
    GI->PlayerState.Wave = 1;
    GI->SaveStageCheckpoint();
    GI->SetShowMainMenuOnStageOneLoad(false);
    bShopOpen = false;
    bBattleActive = false;
    bStageClear = false;
    bGameOver = false;
    bQuestComplete = false;
    CurrentEnemy = nullptr;
    UGameplayStatics::OpenLevel(this, FName(TEXT("Stage02_SunnyMeadow")));
}

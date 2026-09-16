#include "WordQuestHUD.h"
#include "WordQuestGameMode.h"
#include "WordQuestGameInstance.h"
#include "WordQuestEnemy.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWaveProcedural.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

AWordQuestHUD::AWordQuestHUD()
{
    static ConstructorHelpers::FObjectFinder<UFont> FontObject(TEXT("/Engine/EngineFonts/RobotoDistanceField.RobotoDistanceField"));
    HDFont = FontObject.Succeeded() ? FontObject.Object : nullptr;
    MenuBackgroundTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/WordQuestForestBackground.WordQuestForestBackground"));
}

UFont* AWordQuestHUD::GetHUDTextFont() const
{
    if (HDFont) return HDFont;
    return GEngine ? GEngine->GetMediumFont() : nullptr;
}

void AWordQuestHUD::DrawMenuVoxelHero(float CenterX, float TopY, float Scale)
{
    const float HeadW = 74.f * Scale;
    const float HeadH = 70.f * Scale;
    const float BodyW = 82.f * Scale;
    const float BodyH = 92.f * Scale;
    const float LimbW = 26.f * Scale;
    const float ArmH = 82.f * Scale;
    const float LegH = 78.f * Scale;

    const float X = CenterX - HeadW * 0.5f;
    DrawRect(FLinearColor(0.10f, 0.06f, 0.03f, 1.f), X - 5.f * Scale, TopY - 5.f * Scale, HeadW + 10.f * Scale, HeadH + 10.f * Scale);
    DrawRect(FLinearColor(0.45f, 0.25f, 0.14f, 1.f), X, TopY, HeadW, HeadH);
    DrawRect(FLinearColor(0.95f, 0.74f, 0.60f, 1.f), X + 10.f * Scale, TopY + 22.f * Scale, HeadW - 20.f * Scale, HeadH - 24.f * Scale);
    DrawRect(FLinearColor(0.16f, 0.35f, 0.85f, 1.f), X + 14.f * Scale, TopY + 31.f * Scale, 13.f * Scale, 10.f * Scale);
    DrawRect(FLinearColor(0.16f, 0.35f, 0.85f, 1.f), X + 47.f * Scale, TopY + 31.f * Scale, 13.f * Scale, 10.f * Scale);

    const float BodyX = CenterX - BodyW * 0.5f;
    const float BodyY = TopY + HeadH + 6.f * Scale;
    DrawRect(FLinearColor(0.03f, 0.55f, 0.65f, 1.f), BodyX, BodyY, BodyW, BodyH);
    DrawRect(FLinearColor(0.86f, 0.58f, 0.44f, 1.f), BodyX - LimbW, BodyY + 6.f * Scale, LimbW, ArmH);
    DrawRect(FLinearColor(0.86f, 0.58f, 0.44f, 1.f), BodyX + BodyW, BodyY + 6.f * Scale, LimbW, ArmH);

    const float LegY = BodyY + BodyH;
    DrawRect(FLinearColor(0.18f, 0.14f, 0.72f, 1.f), CenterX - LimbW, LegY, LimbW, LegH);
    DrawRect(FLinearColor(0.18f, 0.14f, 0.72f, 1.f), CenterX, LegY, LimbW, LegH);
    DrawRect(FLinearColor(0.15f, 0.15f, 0.15f, 1.f), CenterX - LimbW, LegY + LegH - 10.f * Scale, LimbW, 10.f * Scale);
    DrawRect(FLinearColor(0.15f, 0.15f, 0.15f, 1.f), CenterX, LegY + LegH - 10.f * Scale, LimbW, 10.f * Scale);
}

void AWordQuestHUD::PlayFeedbackTone(float StartFrequency, float EndFrequency, float DurationSeconds, float Volume)
{
    if (!GetWorld()) return;

    constexpr int32 SampleRate = 44100;
    const int32 NumSamples = FMath::Max(1, FMath::RoundToInt(DurationSeconds * SampleRate));
    TArray<int16> PCM;
    PCM.SetNumUninitialized(NumSamples);

    double Phase = 0.0;
    for (int32 i = 0; i < NumSamples; ++i)
    {
        const float T = NumSamples > 1 ? static_cast<float>(i) / static_cast<float>(NumSamples - 1) : 0.f;
        const float Frequency = FMath::Lerp(StartFrequency, EndFrequency, T);
        const float Envelope = FMath::Sin(PI * T);
        Phase += 2.0 * PI * static_cast<double>(Frequency) / static_cast<double>(SampleRate);
        const float Sample = FMath::Sin(static_cast<float>(Phase)) * Envelope;
        PCM[i] = static_cast<int16>(FMath::Clamp(Sample, -1.f, 1.f) * 32760.f);
    }

    USoundWaveProcedural* Sound = NewObject<USoundWaveProcedural>(this);
    if (!Sound) return;

    Sound->SetSampleRate(SampleRate);
    Sound->NumChannels = 1;
    Sound->Duration = DurationSeconds;
    Sound->bLooping = false;
    Sound->QueueAudio(reinterpret_cast<const uint8*>(PCM.GetData()), PCM.Num() * sizeof(int16));

    ActiveFeedbackSounds.Add(Sound);
    UGameplayStatics::PlaySound2D(this, Sound, Volume);

    FTimerHandle CleanupHandle;
    TWeakObjectPtr<AWordQuestHUD> WeakSelf(this);
    TWeakObjectPtr<USoundWaveProcedural> WeakSound(Sound);
    GetWorld()->GetTimerManager().SetTimer(CleanupHandle, [WeakSelf, WeakSound]()
    {
        if (WeakSelf.IsValid() && WeakSound.IsValid()) WeakSelf->ActiveFeedbackSounds.Remove(WeakSound.Get());
    }, DurationSeconds + 0.5f, false);
}

void AWordQuestHUD::AddFloatingMessage(const FString& InText, const FColor& InColor, const FVector& InWorldLocation, float InDuration)
{
    FWordQuestHUDMessage Message;
    Message.Text = InText;
    Message.Color = InColor;
    Message.WorldLocation = InWorldLocation;
    Message.StartTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
    Message.Duration = FMath::Max(0.5f, InDuration);
    FloatingMessages.Add(Message);

    if (InText == TEXT("Correct!")) PlayFeedbackTone(650.f, 980.f, 0.17f, 0.42f);
    else if (InText == TEXT("Wrong!")) PlayFeedbackTone(270.f, 145.f, 0.23f, 0.44f);
}

void AWordQuestHUD::DrawFloatingMessages()
{
    UFont* Font = GetHUDTextFont();
    if (!Canvas || !Font || !GetWorld()) return;

    const float Now = GetWorld()->GetTimeSeconds();
    for (int32 i = FloatingMessages.Num() - 1; i >= 0; --i)
    {
        FWordQuestHUDMessage& Message = FloatingMessages[i];
        const float Age = Now - Message.StartTime;
        if (Age >= Message.Duration)
        {
            FloatingMessages.RemoveAt(i);
            continue;
        }

        FVector2D ScreenPosition(Canvas->SizeX * 0.5f, Canvas->SizeY * 0.40f);
        if (PlayerOwner) PlayerOwner->ProjectWorldLocationToScreen(Message.WorldLocation, ScreenPosition, true);

        const float Progress = FMath::Clamp(Age / Message.Duration, 0.f, 1.f);
        ScreenPosition.Y -= 28.f + Progress * 68.f;
        ScreenPosition.X = FMath::Clamp(ScreenPosition.X, 120.f, Canvas->SizeX - 260.f);
        ScreenPosition.Y = FMath::Clamp(ScreenPosition.Y, 110.f, Canvas->SizeY - 125.f);

        const uint8 Alpha = static_cast<uint8>(255.f * (1.f - Progress));
        FColor TextColor = Message.Color;
        TextColor.A = Alpha;
        const float TextScale = 1.35f;

        Canvas->SetDrawColor(FColor(0, 0, 0, Alpha));
        Canvas->DrawText(Font, Message.Text, ScreenPosition.X + 2.f, ScreenPosition.Y + 2.f, TextScale, TextScale);
        Canvas->SetDrawColor(TextColor);
        Canvas->DrawText(Font, Message.Text, ScreenPosition.X, ScreenPosition.Y, TextScale, TextScale);
    }
}

void AWordQuestHUD::DrawHUD()
{
    Super::DrawHUD();
    UFont* Font = GetHUDTextFont();
    if (!Canvas || !Font) return;

    AWordQuestGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<AWordQuestGameMode>() : nullptr;
    UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>();
    if (!GM || !GI) return;

    const float ScreenW = Canvas->SizeX;
    const float ScreenH = Canvas->SizeY;

    if (GM->bMainMenuOpen)
    {
        if (MenuBackgroundTexture)
        {
            DrawTexture(MenuBackgroundTexture, 0.f, 0.f, ScreenW, ScreenH, 0.f, 0.f, 1.f, 1.f, FLinearColor::White, BLEND_Opaque);
            DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.28f), 0.f, 0.f, ScreenW, ScreenH);
        }
        else
        {
            DrawRect(FLinearColor(0.02f, 0.03f, 0.05f, 0.95f), 0.f, 0.f, ScreenW, ScreenH);
        }

        DrawMenuVoxelHero(ScreenW * 0.20f, ScreenH * 0.34f, 1.15f);

        Canvas->SetDrawColor(FColor::Yellow);
        Canvas->DrawText(Font, TEXT("WORD QUEST"), ScreenW * 0.36f, ScreenH * 0.22f, 3.8f, 3.8f);
        Canvas->SetDrawColor(FColor::White);
        Canvas->DrawText(Font, TEXT("Created by Qazin Khoo"), ScreenW * 0.405f, ScreenH * 0.42f, 1.55f, 1.55f);

        DrawRect(FLinearColor(0.06f, 0.28f, 0.10f, 0.94f), ScreenW * 0.38f, ScreenH * 0.57f, ScreenW * 0.32f, 96.f);
        Canvas->SetDrawColor(FColor::Green);
        Canvas->DrawText(Font, TEXT("START ADVENTURE"), ScreenW * 0.405f, ScreenH * 0.598f, 2.15f, 2.15f);
        Canvas->SetDrawColor(FColor::White);
        Canvas->DrawText(Font, TEXT("Press ENTER"), ScreenW * 0.455f, ScreenH * 0.72f, 1.15f, 1.15f);
        return;
    }

    FString StageName = TEXT("WHISPERING FOREST");
    if (GM->CurrentStage == 2) StageName = TEXT("SUNNY MEADOW");
    else if (GM->CurrentStage == 3) StageName = TEXT("CRYSTAL CAVE");

    if (GM->bGameOver)
    {
        DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.82f), 0.f, 0.f, ScreenW, ScreenH);
        Canvas->SetDrawColor(FColor::Red);
        Canvas->DrawText(Font, TEXT("GAME OVER"), ScreenW * 0.32f, ScreenH * 0.24f, 3.9f, 3.9f);
        Canvas->SetDrawColor(FColor::White);
        Canvas->DrawText(Font, FString::Printf(TEXT("Stage %d - %s"), GM->CurrentStage, *StageName), ScreenW * 0.38f, ScreenH * 0.43f, 1.55f, 1.55f);
        Canvas->DrawText(Font, TEXT("1. Restart Current Stage"), ScreenW * 0.34f, ScreenH * 0.56f, 1.75f, 1.75f);
        Canvas->DrawText(Font, TEXT("2. Main Menu"), ScreenW * 0.40f, ScreenH * 0.65f, 1.75f, 1.75f);
        Canvas->DrawText(Font, TEXT("Press 1 or 2"), ScreenW * 0.425f, ScreenH * 0.76f, 1.15f, 1.15f);
        return;
    }

    if (GM->bShopOpen && !bShopWasOpen) PlayFeedbackTone(520.f, 1100.f, 0.60f, 0.48f);
    bShopWasOpen = GM->bShopOpen;

    DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.62f), 20.f, 20.f, 720.f, 118.f);
    Canvas->SetDrawColor(FColor::White);
    Canvas->DrawText(Font, FString::Printf(TEXT("STAGE %d - %s    WAVE %d/5"), GM->CurrentStage, *StageName, GM->CurrentWave), 35.f, 28.f, 1.10f, 1.10f);
    Canvas->DrawText(Font, FString::Printf(TEXT("HP: %d/%d    COINS: %d    DAMAGE: %d"), GI->PlayerState.CurrentHP, GI->PlayerState.MaxHP, GI->PlayerState.Coins, GI->PlayerState.Damage), 35.f, 67.f, 1.08f, 1.08f);
    Canvas->DrawText(Font, GI->PlayerState.bHasSword ? TEXT("SWORD: COLLECTED") : TEXT("SWORD: FIND IT AHEAD"), 35.f, 101.f, 1.00f, 1.00f);

    if (GM->bShopOpen)
    {
        const float PanelX = ScreenW * 0.09f;
        const float PanelY = ScreenH * 0.15f;
        const float PanelW = ScreenW * 0.82f;
        const float PanelH = 445.f;
        DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.90f), PanelX, PanelY, PanelW, PanelH);

        Canvas->SetDrawColor(FColor::Yellow);
        Canvas->DrawText(Font, FString::Printf(TEXT("STAGE %d CLEAR - SHOP"), GM->CurrentStage), PanelX + 36.f, PanelY + 18.f, 1.20f, 1.20f);
        Canvas->SetDrawColor(FColor::White);
        Canvas->DrawText(Font, FString::Printf(TEXT("Coins: %d"), GI->PlayerState.Coins), PanelX + 36.f, PanelY + 76.f, 1.10f, 1.10f);
        Canvas->DrawText(Font, TEXT("Each item can be bought once per shop visit."), PanelX + 225.f, PanelY + 80.f, 0.90f, 0.90f);

        auto ShopStatus = [](bool bBought, bool bCanBuy, const TCHAR* ReadyText, const TCHAR* UnavailableText)
        {
            if (bBought) return FString(TEXT("SOLD"));
            return bCanBuy ? FString(ReadyText) : FString(UnavailableText);
        };

        const FString AppleStatus = ShopStatus(GI->IsAppleBoughtThisVisit(), GI->CanBuyApple(), TEXT("READY"), GI->PlayerState.CurrentHP >= GI->PlayerState.MaxHP ? TEXT("FULL HP") : TEXT("NEED 3 COINS"));
        const FString StarStatus = ShopStatus(GI->IsStarBoughtThisVisit(), GI->CanBuyStar(), TEXT("READY"), TEXT("NEED 10 COINS"));
        const FString ArmourStatus = ShopStatus(GI->IsArmourBoughtThisVisit(), GI->CanBuyArmour(), TEXT("READY"), TEXT("NEED 25 COINS"));

        Canvas->SetDrawColor(GI->CanBuyApple() ? FColor::Green : FColor::Silver);
        Canvas->DrawText(Font, FString::Printf(TEXT("1. APPLE   3 COINS   +1 HP   [%s]"), *AppleStatus), PanelX + 48.f, PanelY + 150.f, 1.05f, 1.05f);
        Canvas->SetDrawColor(GI->CanBuyStar() ? FColor::Cyan : FColor::Silver);
        Canvas->DrawText(Font, FString::Printf(TEXT("2. STAR   10 COINS   BLOCKS NEXT 2 HITS   [%s]"), *StarStatus), PanelX + 48.f, PanelY + 208.f, 1.05f, 1.05f);
        Canvas->SetDrawColor(GI->CanBuyArmour() ? FColor::Yellow : FColor::Silver);
        Canvas->DrawText(Font, FString::Printf(TEXT("3. ARMOUR   25 COINS   +10 MAX HP & +10 HP   [%s]"), *ArmourStatus), PanelX + 48.f, PanelY + 266.f, 1.05f, 1.05f);
        Canvas->SetDrawColor(FColor::White);
        Canvas->DrawText(Font, TEXT("1 / 2 / 3: Buy      4 or ENTER: Continue"), PanelX + 36.f, PanelY + 370.f, 1.00f, 1.00f);
        DrawFloatingMessages();
        return;
    }

    if (GM->bStageClear)
    {
        DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.78f), ScreenW * 0.18f, ScreenH * 0.30f, ScreenW * 0.64f, 188.f);
        Canvas->SetDrawColor(FColor::Yellow);
        Canvas->DrawText(Font, FString::Printf(TEXT("STAGE %d COMPLETE"), GM->CurrentStage), ScreenW * 0.33f, ScreenH * 0.34f, 1.25f, 1.25f);
        Canvas->SetDrawColor(FColor::White);
        Canvas->DrawText(Font, TEXT("Continue through the shop to the next stage."), ScreenW * 0.28f, ScreenH * 0.45f, 1.00f, 1.00f);
        DrawFloatingMessages();
        return;
    }

    if (!GM->bBattleActive)
    {
        DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.50f), ScreenW * 0.24f, ScreenH - 82.f, ScreenW * 0.52f, 52.f);
        Canvas->SetDrawColor(FColor::White);
        Canvas->DrawText(Font, TEXT("A / D or Arrow Keys: Move     Space: Jump"), ScreenW * 0.285f, ScreenH - 69.f, 0.95f, 0.95f);
        DrawFloatingMessages();
        return;
    }

    const float PanelX = ScreenW * 0.045f;
    const float PanelY = ScreenH * 0.565f;
    const float PanelW = ScreenW * 0.91f;
    const float PanelH = 355.f;
    DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.88f), PanelX, PanelY, PanelW, PanelH);

    const bool bBossBattle = GM->CurrentEnemy && GM->CurrentEnemy->bBoss;
    const FString BattleTitle = bBossBattle ? TEXT("BOSS BATTLE") : TEXT("WORD BATTLE");
    Canvas->SetDrawColor(FColor::Yellow);
    Canvas->DrawText(Font, BattleTitle, PanelX + 26.f, PanelY + 6.f, 1.40f, 1.40f);

    Canvas->SetDrawColor(FColor::White);
    Canvas->DrawText(Font, GM->CurrentQuestion.Prompt, PanelX + 26.f, PanelY + 43.f, 2.50f, 2.50f);

    const float AnswerStartY = PanelY + 112.f;
    const float AnswerGap = 51.f;
    const float FeedbackProgress = GM->bAnswerFeedbackActive && GetWorld() ? FMath::Clamp(GetWorld()->GetTimeSeconds() - GM->AnswerFeedbackStartTime, 0.f, 1.f) : 0.f;

    for (int32 i = 0; i < GM->CurrentQuestion.Answers.Num() && i < 4; ++i)
    {
        float X = PanelX + 52.f;
        float Y = AnswerStartY + i * AnswerGap;
        FColor AnswerColor = FColor::White;

        if (GM->bAnswerFeedbackActive && i == GM->SelectedAnswerIndex)
        {
            if (GM->bSelectedAnswerCorrect)
            {
                AnswerColor = FColor::Green;
                Y -= FeedbackProgress * 32.f;
                DrawRect(FLinearColor(0.05f, 0.35f, 0.08f, 0.70f), X - 14.f, Y - 6.f, PanelW * 0.74f, 46.f);
            }
            else
            {
                AnswerColor = FColor::Red;
                X += FMath::Sin(FeedbackProgress * PI * 10.f) * 18.f;
                DrawRect(FLinearColor(0.45f, 0.04f, 0.04f, 0.70f), X - 14.f, Y - 6.f, PanelW * 0.74f, 46.f);
            }
        }

        Canvas->SetDrawColor(AnswerColor);
        Canvas->DrawText(Font, FString::Printf(TEXT("%d. %s"), i + 1, *GM->CurrentQuestion.Answers[i]), X, Y, 2.25f, 2.25f);
    }

    Canvas->SetDrawColor(GM->bAnswerFeedbackActive ? FColor::Silver : FColor::Green);
    Canvas->DrawText(Font, GM->bAnswerFeedbackActive ? TEXT("Checking answer...") : TEXT("Press 1, 2, 3 or 4 to answer"), PanelX + 26.f, PanelY + 320.f, 1.30f, 1.30f);
    DrawFloatingMessages();
}

#include "WordQuestHUD.h"
#include "WordQuestGameMode.h"
#include "WordQuestGameInstance.h"
#include "WordQuestEnemy.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWaveProcedural.h"
#include "TimerManager.h"

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
        if (WeakSelf.IsValid() && WeakSound.IsValid())
        {
            WeakSelf->ActiveFeedbackSounds.Remove(WeakSound.Get());
        }
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
    if (!Canvas || !GEngine || !GetWorld()) return;

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
        FColor ShadowColor(0, 0, 0, Alpha);
        FColor TextColor = Message.Color;
        TextColor.A = Alpha;
        const float TextScale = 2.25f;

        Canvas->SetDrawColor(ShadowColor);
        Canvas->DrawText(GEngine->GetLargeFont(), Message.Text, ScreenPosition.X + 3.f, ScreenPosition.Y + 3.f, TextScale, TextScale);
        Canvas->SetDrawColor(TextColor);
        Canvas->DrawText(GEngine->GetLargeFont(), Message.Text, ScreenPosition.X, ScreenPosition.Y, TextScale, TextScale);
    }
}

void AWordQuestHUD::DrawHUD()
{
    Super::DrawHUD();
    if (!Canvas || !GEngine) return;

    AWordQuestGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<AWordQuestGameMode>() : nullptr;
    UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>();
    if (!GM || !GI) return;

    const float ScreenW = Canvas->SizeX;
    const float ScreenH = Canvas->SizeY;
    FString StageName = TEXT("WHISPERING FOREST");
    if (GM->CurrentStage == 2) StageName = TEXT("SUNNY MEADOW");
    else if (GM->CurrentStage == 3) StageName = TEXT("CRYSTAL CAVE");

    if (GM->bShopOpen && !bShopWasOpen) PlayFeedbackTone(520.f, 1100.f, 0.60f, 0.48f);
    bShopWasOpen = GM->bShopOpen;

    DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.62f), 20.f, 20.f, 720.f, 118.f);
    Canvas->SetDrawColor(FColor::White);
    Canvas->DrawText(GEngine->GetSmallFont(), FString::Printf(TEXT("STAGE %d - %s    WAVE %d/5"), GM->CurrentStage, *StageName, GM->CurrentWave), 35.f, 28.f, 1.95f, 1.95f);
    Canvas->DrawText(GEngine->GetSmallFont(), FString::Printf(TEXT("HP: %d/%d    COINS: %d    DAMAGE: %d"), GI->PlayerState.CurrentHP, GI->PlayerState.MaxHP, GI->PlayerState.Coins, GI->PlayerState.Damage), 35.f, 67.f, 1.90f, 1.90f);
    Canvas->DrawText(GEngine->GetSmallFont(), GI->PlayerState.bHasSword ? TEXT("SWORD: COLLECTED") : TEXT("SWORD: FIND IT AHEAD"), 35.f, 101.f, 1.70f, 1.70f);

    if (GM->bShopOpen)
    {
        const float PanelX = ScreenW * 0.09f;
        const float PanelY = ScreenH * 0.15f;
        const float PanelW = ScreenW * 0.82f;
        const float PanelH = 445.f;
        DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.90f), PanelX, PanelY, PanelW, PanelH);

        Canvas->SetDrawColor(FColor::Yellow);
        Canvas->DrawText(GEngine->GetLargeFont(), FString::Printf(TEXT("STAGE %d CLEAR - SHOP"), GM->CurrentStage), PanelX + 36.f, PanelY + 18.f, 1.70f, 1.70f);

        Canvas->SetDrawColor(FColor::White);
        Canvas->DrawText(GEngine->GetSmallFont(), FString::Printf(TEXT("Coins: %d"), GI->PlayerState.Coins), PanelX + 36.f, PanelY + 76.f, 1.90f, 1.90f);
        Canvas->DrawText(GEngine->GetSmallFont(), TEXT("Each item can be bought once per shop visit."), PanelX + 225.f, PanelY + 80.f, 1.50f, 1.50f);

        auto ShopStatus = [](bool bBought, bool bCanBuy, const TCHAR* ReadyText, const TCHAR* UnavailableText)
        {
            if (bBought) return FString(TEXT("SOLD"));
            return bCanBuy ? FString(ReadyText) : FString(UnavailableText);
        };

        const FString AppleStatus = ShopStatus(GI->IsAppleBoughtThisVisit(), GI->CanBuyApple(), TEXT("READY"), GI->PlayerState.CurrentHP >= GI->PlayerState.MaxHP ? TEXT("FULL HP") : TEXT("NEED 3 COINS"));
        const FString StarStatus = ShopStatus(GI->IsStarBoughtThisVisit(), GI->CanBuyStar(), TEXT("READY"), TEXT("NEED 10 COINS"));
        const FString ArmourStatus = ShopStatus(GI->IsArmourBoughtThisVisit(), GI->CanBuyArmour(), TEXT("READY"), TEXT("NEED 25 COINS"));

        Canvas->SetDrawColor(GI->CanBuyApple() ? FColor::Green : FColor::Silver);
        Canvas->DrawText(GEngine->GetSmallFont(), FString::Printf(TEXT("1. APPLE   3 COINS   +1 HP   [%s]"), *AppleStatus), PanelX + 48.f, PanelY + 150.f, 1.85f, 1.85f);
        Canvas->SetDrawColor(GI->CanBuyStar() ? FColor::Cyan : FColor::Silver);
        Canvas->DrawText(GEngine->GetSmallFont(), FString::Printf(TEXT("2. STAR   10 COINS   BLOCKS NEXT 2 HITS   [%s]"), *StarStatus), PanelX + 48.f, PanelY + 208.f, 1.85f, 1.85f);
        Canvas->SetDrawColor(GI->CanBuyArmour() ? FColor::Yellow : FColor::Silver);
        Canvas->DrawText(GEngine->GetSmallFont(), FString::Printf(TEXT("3. ARMOUR   25 COINS   +10 MAX HP & +10 HP   [%s]"), *ArmourStatus), PanelX + 48.f, PanelY + 266.f, 1.85f, 1.85f);

        Canvas->SetDrawColor(FColor::White);
        Canvas->DrawText(GEngine->GetSmallFont(), TEXT("1 / 2 / 3: Buy      4 or ENTER: Continue"), PanelX + 36.f, PanelY + 370.f, 1.70f, 1.70f);
        DrawFloatingMessages();
        return;
    }

    if (GM->bStageClear)
    {
        DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.78f), ScreenW * 0.18f, ScreenH * 0.30f, ScreenW * 0.64f, 188.f);
        Canvas->SetDrawColor(FColor::Yellow);
        Canvas->DrawText(GEngine->GetLargeFont(), FString::Printf(TEXT("STAGE %d COMPLETE"), GM->CurrentStage), ScreenW * 0.33f, ScreenH * 0.34f, 1.95f, 1.95f);
        Canvas->SetDrawColor(FColor::White);
        Canvas->DrawText(GEngine->GetSmallFont(), TEXT("More stages are being added."), ScreenW * 0.34f, ScreenH * 0.45f, 1.75f, 1.75f);
        DrawFloatingMessages();
        return;
    }

    if (!GM->bBattleActive)
    {
        DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.50f), ScreenW * 0.24f, ScreenH - 82.f, ScreenW * 0.52f, 52.f);
        Canvas->SetDrawColor(FColor::White);
        Canvas->DrawText(GEngine->GetSmallFont(), TEXT("A / D or Arrow Keys: Move     Space: Jump"), ScreenW * 0.285f, ScreenH - 69.f, 1.65f, 1.65f);
        DrawFloatingMessages();
        return;
    }

    const float PanelX = ScreenW * 0.08f;
    const float PanelY = ScreenH * 0.64f;
    const float PanelW = ScreenW * 0.84f;
    const float PanelH = 252.f;
    DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.86f), PanelX, PanelY, PanelW, PanelH);

    const bool bBossBattle = GM->CurrentEnemy && GM->CurrentEnemy->bBoss;
    const FString BattleTitle = bBossBattle ? TEXT("BOSS BATTLE") : TEXT("WORD BATTLE");

    Canvas->SetDrawColor(FColor::Yellow);
    Canvas->DrawText(GEngine->GetLargeFont(), BattleTitle, PanelX + 28.f, PanelY + 8.f, 1.70f, 1.70f);

    Canvas->SetDrawColor(FColor::White);
    Canvas->DrawText(GEngine->GetSmallFont(), GM->CurrentQuestion.Prompt, PanelX + 28.f, PanelY + 48.f, 2.15f, 2.15f);

    const float AnswerStartY = PanelY + 95.f;
    const float AnswerGap = 31.f;
    for (int32 i = 0; i < GM->CurrentQuestion.Answers.Num() && i < 4; ++i)
    {
        const float Y = AnswerStartY + i * AnswerGap;
        Canvas->DrawText(GEngine->GetSmallFont(), FString::Printf(TEXT("%d. %s"), i + 1, *GM->CurrentQuestion.Answers[i]), PanelX + 52.f, Y, 1.90f, 1.90f);
    }

    Canvas->SetDrawColor(FColor::Green);
    Canvas->DrawText(GEngine->GetSmallFont(), TEXT("Press 1, 2, 3 or 4 to answer"), PanelX + 28.f, PanelY + 218.f, 1.60f, 1.60f);
    DrawFloatingMessages();
}

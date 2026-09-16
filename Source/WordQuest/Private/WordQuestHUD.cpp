#include "WordQuestHUD.h"
#include "WordQuestGameMode.h"
#include "WordQuestGameInstance.h"
#include "WordQuestEnemy.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "Engine/FontFace.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "Fonts/CompositeFont.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWaveProcedural.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

AWordQuestHUD::AWordQuestHUD()
{
    static ConstructorHelpers::FObjectFinder<UFont> FallbackFontObject(TEXT("/Engine/EngineFonts/RobotoDistanceField.RobotoDistanceField"));
    HDFont = FallbackFontObject.Succeeded() ? FallbackFontObject.Object : nullptr;

    // The new introduction is a complete title-screen composition. We draw it
    // directly, without the previous extra title, button and placeholder hero.
    MenuBackgroundTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/WordQuestIntro.WordQuestIntro"));

    // Kept only for the legacy helper functions. Gameplay uses the world-space
    // character component, not this HUD sprite.
    HeroSpriteTexture = nullptr;
}

void AWordQuestHUD::BeginPlay()
{
    Super::BeginPlay();

    // Importing a TTF in Unreal creates a Font Face. Build a runtime UFont from
    // that Font Face so the existing Canvas HUD can use the same Word Quest font
    // everywhere without requiring another manually-created Font asset.
    UFontFace* WordQuestFontFace = LoadObject<UFontFace>(nullptr, TEXT("/Game/UI/Fonts/WordQuestUIFont.WordQuestUIFont"));
    if (WordQuestFontFace)
    {
        UFont* RuntimeFont = NewObject<UFont>(this, TEXT("WordQuestRuntimeFont"));
        if (RuntimeFont)
        {
            RuntimeFont->FontCacheType = EFontCacheType::Runtime;
            RuntimeFont->LegacyFontName = FName(TEXT("Regular"));
            RuntimeFont->LegacyFontSize = 24;
            RuntimeFont->CompositeFont.DefaultTypeface.Fonts.Reset();

            FTypefaceEntry RegularEntry(FName(TEXT("Regular")));
            RegularEntry.Font = FFontData(WordQuestFontFace, 0);
            RuntimeFont->CompositeFont.DefaultTypeface.Fonts.Add(RegularEntry);
            RuntimeFont->CompositeFont.MakeDirty();

            HDFont = RuntimeFont;
        }
    }
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

void AWordQuestHUD::DrawMenuHeroSprite(float X, float Y, float W, float H)
{
    if (!HeroSpriteTexture)
    {
        DrawMenuVoxelHero(X + W * 0.5f, Y, 1.12f);
        return;
    }

    DrawTexture(HeroSpriteTexture, X, Y, W, H, 0.f, 0.f, 0.25f, 1.f, FLinearColor::White, BLEND_Translucent);
}

void AWordQuestHUD::DrawGameplayHeroSprite()
{
    // Gameplay hero is rendered by AWordQuestCharacter's world-space widget.
    // This old HUD overlay is intentionally disabled.
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
    else if (InText.StartsWith(TEXT("Wrong!"))) PlayFeedbackTone(270.f, 145.f, 0.23f, 0.44f);
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

    auto DrawBlockText = [this, Font](const FString& Text, float X, float Y, float Scale, const FColor& MainColor)
    {
        Canvas->SetDrawColor(MainColor);
        Canvas->DrawText(Font, Text, X, Y, Scale, Scale);
    };

    auto DrawCenteredBlockText = [this, Font, &DrawBlockText](const FString& Text, float CenterX, float Y, float Scale, const FColor& MainColor)
    {
        float XL = 0.f;
        float YL = 0.f;
        Canvas->StrLen(Font, Text, XL, YL);
        DrawBlockText(Text, CenterX - (XL * Scale * 0.5f), Y, Scale, MainColor);
    };

    if (GM->bMainMenuOpen)
    {
        if (MenuBackgroundTexture)
        {
            DrawTexture(MenuBackgroundTexture, 0.f, 0.f, ScreenW, ScreenH, 0.f, 0.f, 1.f, 1.f, FLinearColor::White, BLEND_Opaque);
        }
        else
        {
            DrawRect(FLinearColor(0.02f, 0.03f, 0.05f, 0.98f), 0.f, 0.f, ScreenW, ScreenH);
            DrawCenteredBlockText(TEXT("WORD QUEST"), ScreenW * 0.5f, ScreenH * 0.28f, 2.6f, FColor(255, 226, 72));
            DrawCenteredBlockText(TEXT("START QUEST"), ScreenW * 0.5f, ScreenH * 0.58f, 1.55f, FColor(255, 226, 72));
        }
        return;
    }

    FString StageName = TEXT("WHISPERING FOREST");
    if (GM->CurrentStage == 2) StageName = TEXT("SUNNY MEADOW");
    else if (GM->CurrentStage == 3) StageName = TEXT("CRYSTAL CAVE");

    if (GM->bGameOver)
    {
        DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.82f), 0.f, 0.f, ScreenW, ScreenH);
        DrawCenteredBlockText(TEXT("GAME OVER"), ScreenW * 0.5f, ScreenH * 0.22f, 3.5f, FColor::Red);
        DrawCenteredBlockText(FString::Printf(TEXT("STAGE %d - %s"), GM->CurrentStage, *StageName), ScreenW * 0.5f, ScreenH * 0.42f, 1.35f, FColor::White);
        DrawCenteredBlockText(TEXT("1. RESTART CURRENT STAGE"), ScreenW * 0.5f, ScreenH * 0.56f, 1.55f, FColor::White);
        DrawCenteredBlockText(TEXT("2. MAIN MENU"), ScreenW * 0.5f, ScreenH * 0.65f, 1.55f, FColor::White);
        DrawCenteredBlockText(TEXT("PRESS 1 OR 2"), ScreenW * 0.5f, ScreenH * 0.76f, 1.00f, FColor::Silver);
        return;
    }

    if (GM->bShopOpen && !bShopWasOpen) PlayFeedbackTone(520.f, 1100.f, 0.60f, 0.48f);
    bShopWasOpen = GM->bShopOpen;

    DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.68f), 20.f, 20.f, 760.f, 150.f);
    DrawBlockText(FString::Printf(TEXT("STAGE %d - %s    WAVE %d/5"), GM->CurrentStage, *StageName, GM->CurrentWave), 35.f, 28.f, 1.05f, FColor::White);
    DrawBlockText(FString::Printf(TEXT("HP: %d/%d"), GI->PlayerState.CurrentHP, GI->PlayerState.MaxHP), 35.f, 67.f, 1.05f, FColor(80, 255, 80));
    DrawBlockText(FString::Printf(TEXT("COINS: %d"), GI->PlayerState.Coins), 190.f, 67.f, 1.05f, FColor(255, 220, 60));
    DrawBlockText(FString::Printf(TEXT("DAMAGE: %d"), GI->PlayerState.Damage), 350.f, 67.f, 1.05f, FColor(255, 90, 90));
    DrawBlockText(GI->PlayerState.bHasSword ? TEXT("SWORD: COLLECTED") : TEXT("SWORD: FIND IT AHEAD"), 35.f, 101.f, 0.95f, FColor::White);
    DrawBlockText(FString::Printf(TEXT("STAR BLOCKS: %d"), GI->PlayerState.StarProtectionHits), 35.f, 128.f, 0.95f, GI->PlayerState.StarProtectionHits > 0 ? FColor::Cyan : FColor::Silver);

    if (GM->bShopOpen)
    {
        const float PanelX = ScreenW * 0.09f;
        const float PanelY = ScreenH * 0.15f;
        const float PanelW = ScreenW * 0.82f;
        const float PanelH = 445.f;
        DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.90f), PanelX, PanelY, PanelW, PanelH);

        DrawBlockText(FString::Printf(TEXT("STAGE %d CLEAR - SHOP"), GM->CurrentStage), PanelX + 36.f, PanelY + 18.f, 1.20f, FColor::Yellow);
        DrawBlockText(FString::Printf(TEXT("Coins: %d"), GI->PlayerState.Coins), PanelX + 36.f, PanelY + 76.f, 1.10f, FColor::White);
        DrawBlockText(TEXT("Each item can be bought once per shop visit."), PanelX + 225.f, PanelY + 80.f, 0.90f, FColor::White);

        auto ShopStatus = [](bool bBought, bool bCanBuy, const TCHAR* ReadyText, const TCHAR* UnavailableText)
        {
            if (bBought) return FString(TEXT("SOLD"));
            return bCanBuy ? FString(ReadyText) : FString(UnavailableText);
        };

        const FString AppleStatus = ShopStatus(GI->IsAppleBoughtThisVisit(), GI->CanBuyApple(), TEXT("READY"), GI->PlayerState.CurrentHP >= GI->PlayerState.MaxHP ? TEXT("FULL HP") : TEXT("NEED 3 COINS"));
        const FString StarStatus = ShopStatus(GI->IsStarBoughtThisVisit(), GI->CanBuyStar(), TEXT("READY"), TEXT("NEED 10 COINS"));
        const FString ArmourStatus = ShopStatus(GI->IsArmourBoughtThisVisit(), GI->CanBuyArmour(), TEXT("READY"), TEXT("NEED 25 COINS"));

        DrawBlockText(FString::Printf(TEXT("1. APPLE   3 COINS   +1 HP   [%s]"), *AppleStatus), PanelX + 48.f, PanelY + 150.f, 1.05f, GI->CanBuyApple() ? FColor::Green : FColor::Silver);
        DrawBlockText(FString::Printf(TEXT("2. STAR   10 COINS   BLOCKS NEXT 2 HITS   [%s]"), *StarStatus), PanelX + 48.f, PanelY + 208.f, 1.05f, GI->CanBuyStar() ? FColor::Cyan : FColor::Silver);
        DrawBlockText(FString::Printf(TEXT("3. ARMOUR   25 COINS   +10 MAX HP & +10 HP   [%s]"), *ArmourStatus), PanelX + 48.f, PanelY + 266.f, 1.05f, GI->CanBuyArmour() ? FColor::Yellow : FColor::Silver);
        DrawBlockText(TEXT("1 / 2 / 3: BUY      4 OR ENTER: CONTINUE"), PanelX + 36.f, PanelY + 370.f, 1.00f, FColor::White);
        DrawFloatingMessages();
        return;
    }

    if (GM->bStageClear)
    {
        DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.78f), ScreenW * 0.18f, ScreenH * 0.30f, ScreenW * 0.64f, 188.f);
        DrawCenteredBlockText(FString::Printf(TEXT("STAGE %d COMPLETE"), GM->CurrentStage), ScreenW * 0.5f, ScreenH * 0.34f, 1.30f, FColor::Yellow);
        DrawCenteredBlockText(TEXT("CONTINUE THROUGH THE SHOP TO THE NEXT STAGE"), ScreenW * 0.5f, ScreenH * 0.45f, 0.95f, FColor::White);
        DrawFloatingMessages();
        return;
    }

    if (!GM->bBattleActive)
    {
        DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.50f), ScreenW * 0.24f, ScreenH - 82.f, ScreenW * 0.52f, 52.f);
        DrawCenteredBlockText(TEXT("A / D OR ARROWS: MOVE       SPACE: JUMP"), ScreenW * 0.5f, ScreenH - 69.f, 0.90f, FColor::White);
        DrawFloatingMessages();
        return;
    }

    const float PanelX = ScreenW * 0.055f;
    const float PanelY = ScreenH * 0.57f;
    const float PanelW = ScreenW * 0.89f;
    const float PanelH = ScreenH * 0.39f;
    DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.90f), PanelX, PanelY, PanelW, PanelH);

    const bool bBossBattle = GM->CurrentEnemy && GM->CurrentEnemy->bBoss;
    DrawBlockText(bBossBattle ? TEXT("BOSS BATTLE") : TEXT("WORD BATTLE"), PanelX + 22.f, PanelY + 5.f, 1.14f, FColor::Yellow);

    if (GM->CurrentEnemy)
    {
        const int32 EnemyHP = GM->CurrentEnemy->CurrentHP;
        const int32 EnemyMaxHP = GM->CurrentEnemy->MaxHP;
        const float BarW = 220.f;
        const float BarH = 18.f;
        const float BarX = PanelX + PanelW - BarW - 28.f;
        const float BarY = PanelY + 15.f;
        const float FillPercent = EnemyMaxHP > 0 ? static_cast<float>(EnemyHP) / static_cast<float>(EnemyMaxHP) : 0.f;

        DrawRect(FLinearColor(0.10f, 0.10f, 0.10f, 1.f), BarX - 2.f, BarY - 2.f, BarW + 4.f, BarH + 4.f);
        DrawRect(FLinearColor(0.30f, 0.04f, 0.04f, 1.f), BarX, BarY, BarW, BarH);
        DrawRect(FLinearColor(0.92f, 0.16f, 0.16f, 1.f), BarX, BarY, BarW * FMath::Clamp(FillPercent, 0.f, 1.f), BarH);
        DrawBlockText(FString::Printf(TEXT("ENEMY HP %d/%d"), EnemyHP, EnemyMaxHP), BarX, BarY + 23.f, 0.82f, FColor::White);
    }

    const float PromptScale = 1.70f;
    const float MaxPromptWidth = PanelW - 54.f;
    FString Line1 = GM->CurrentQuestion.Prompt;
    FString Line2;

    float FullTextWidth = 0.f;
    float FullTextHeight = 0.f;
    Canvas->StrLen(Font, Line1, FullTextWidth, FullTextHeight);

    if (FullTextWidth * PromptScale > MaxPromptWidth)
    {
        int32 BestBreak = INDEX_NONE;
        for (int32 CharIndex = 0; CharIndex < Line1.Len(); ++CharIndex)
        {
            if (Line1[CharIndex] != TCHAR(' ')) continue;

            const FString Candidate = Line1.Left(CharIndex);
            float CandidateWidth = 0.f;
            float CandidateHeight = 0.f;
            Canvas->StrLen(Font, Candidate, CandidateWidth, CandidateHeight);

            if (CandidateWidth * PromptScale <= MaxPromptWidth)
            {
                BestBreak = CharIndex;
            }
            else
            {
                break;
            }
        }

        if (BestBreak != INDEX_NONE)
        {
            Line2 = Line1.Mid(BestBreak + 1);
            Line1 = Line1.Left(BestBreak);
        }
    }

    DrawBlockText(Line1, PanelX + 22.f, PanelY + 52.f, PromptScale, FColor::White);
    if (!Line2.IsEmpty())
    {
        DrawBlockText(Line2, PanelX + 22.f, PanelY + 95.f, PromptScale, FColor::White);
    }

    const float AnswerScale = 1.55f;
    const float AnswerStartY = PanelY + (Line2.IsEmpty() ? 114.f : 151.f);
    const float AnswerGap = 43.f;
    const float FeedbackProgress = GM->bAnswerFeedbackActive && GetWorld() ? FMath::Clamp(GetWorld()->GetTimeSeconds() - GM->AnswerFeedbackStartTime, 0.f, 1.f) : 0.f;

    for (int32 i = 0; i < GM->CurrentQuestion.Answers.Num() && i < 4; ++i)
    {
        float X = PanelX + 48.f;
        float Y = AnswerStartY + i * AnswerGap;
        FColor AnswerColor = FColor::White;

        if (GM->bAnswerFeedbackActive && i == GM->SelectedAnswerIndex)
        {
            if (GM->bSelectedAnswerCorrect)
            {
                AnswerColor = FColor::Green;
                Y -= FeedbackProgress * 22.f;
                DrawRect(FLinearColor(0.05f, 0.35f, 0.08f, 0.72f), X - 12.f, Y - 3.f, PanelW * 0.72f, 39.f);
            }
            else
            {
                AnswerColor = FColor::Red;
                X += FMath::Sin(FeedbackProgress * PI * 10.f) * 16.f;
                DrawRect(FLinearColor(0.45f, 0.04f, 0.04f, 0.72f), X - 12.f, Y - 3.f, PanelW * 0.72f, 39.f);
            }
        }

        DrawBlockText(FString::Printf(TEXT("%d. %s"), i + 1, *GM->CurrentQuestion.Answers[i]), X, Y, AnswerScale, AnswerColor);
    }

    DrawBlockText(GM->bAnswerFeedbackActive ? TEXT("CHECKING...") : TEXT("PRESS 1, 2, 3 OR 4 TO ANSWER"), PanelX + 22.f, PanelY + PanelH - 36.f, 1.02f, GM->bAnswerFeedbackActive ? FColor::Silver : FColor::Green);
    DrawFloatingMessages();
}

#include "WordQuestHUD.h"
#include "WordQuestGameMode.h"
#include "WordQuestGameInstance.h"
#include "WordQuestEnemy.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

void AWordQuestHUD::AddFloatingMessage(const FString& InText, const FColor& InColor, const FVector& InWorldLocation, float InDuration)
{
    FWordQuestHUDMessage Message;
    Message.Text = InText;
    Message.Color = InColor;
    Message.WorldLocation = InWorldLocation;
    Message.StartTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
    Message.Duration = FMath::Max(0.5f, InDuration);
    FloatingMessages.Add(Message);
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

        FVector2D ScreenPosition(Canvas->SizeX * 0.5f, Canvas->SizeY * 0.42f);
        if (PlayerOwner)
        {
            PlayerOwner->ProjectWorldLocationToScreen(Message.WorldLocation, ScreenPosition, true);
        }

        const float Progress = FMath::Clamp(Age / Message.Duration, 0.f, 1.f);
        ScreenPosition.Y -= 45.f + Progress * 85.f;
        ScreenPosition.X = FMath::Clamp(ScreenPosition.X, 120.f, Canvas->SizeX - 120.f);
        ScreenPosition.Y = FMath::Clamp(ScreenPosition.Y, 120.f, Canvas->SizeY - 140.f);

        const uint8 Alpha = static_cast<uint8>(255.f * (1.f - Progress));
        FColor ShadowColor(0, 0, 0, Alpha);
        FColor TextColor = Message.Color;
        TextColor.A = Alpha;

        const float TextScale = 1.75f;
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

    DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.62f), 20.f, 20.f, 600.f, 135.f);
    Canvas->SetDrawColor(FColor::White);
    Canvas->DrawText(GEngine->GetSmallFont(), FString::Printf(TEXT("STAGE 1 - WHISPERING FOREST    WAVE %d/5"), GM->CurrentWave), 35.f, 32.f, 1.45f, 1.45f);
    Canvas->DrawText(GEngine->GetSmallFont(), FString::Printf(TEXT("HP: %d/%d    COINS: %d    DAMAGE: %d"), GI->PlayerState.CurrentHP, GI->PlayerState.MaxHP, GI->PlayerState.Coins, GI->PlayerState.Damage), 35.f, 76.f, 1.45f, 1.45f);
    Canvas->DrawText(GEngine->GetSmallFont(), GI->PlayerState.bHasSword ? TEXT("SWORD: COLLECTED") : TEXT("SWORD: FIND IT AHEAD"), 35.f, 118.f, 1.30f, 1.30f);

    if (GM->bShopOpen)
    {
        const float PanelX = ScreenW * 0.12f;
        const float PanelY = ScreenH * 0.14f;
        const float PanelW = ScreenW * 0.76f;
        const float PanelH = 520.f;

        DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.90f), PanelX, PanelY, PanelW, PanelH);

        Canvas->SetDrawColor(FColor::Yellow);
        Canvas->DrawText(GEngine->GetLargeFont(), TEXT("STAGE 1 CLEAR - SHOP"), PanelX + 36.f, PanelY + 24.f, 1.35f, 1.35f);

        Canvas->SetDrawColor(FColor::White);
        Canvas->DrawText(GEngine->GetSmallFont(), FString::Printf(TEXT("Coins available: %d"), GI->PlayerState.Coins), PanelX + 36.f, PanelY + 92.f, 1.50f, 1.50f);
        Canvas->DrawText(GEngine->GetSmallFont(), TEXT("Each item may be bought once during this shop visit."), PanelX + 36.f, PanelY + 132.f, 1.30f, 1.30f);

        auto ShopStatus = [](bool bBought, bool bCanBuy, const TCHAR* ReadyText, const TCHAR* UnavailableText)
        {
            if (bBought) return FString(TEXT("SOLD"));
            return bCanBuy ? FString(ReadyText) : FString(UnavailableText);
        };

        const FString AppleStatus = ShopStatus(GI->IsAppleBoughtThisVisit(), GI->CanBuyApple(), TEXT("READY"), GI->PlayerState.CurrentHP >= GI->PlayerState.MaxHP ? TEXT("FULL HP") : TEXT("NEED 3 COINS"));
        const FString StarStatus = ShopStatus(GI->IsStarBoughtThisVisit(), GI->CanBuyStar(), TEXT("READY"), TEXT("NEED 10 COINS"));
        const FString ArmourStatus = ShopStatus(GI->IsArmourBoughtThisVisit(), GI->CanBuyArmour(), TEXT("READY"), TEXT("NEED 25 COINS"));

        Canvas->SetDrawColor(GI->CanBuyApple() ? FColor::Green : FColor::Silver);
        Canvas->DrawText(GEngine->GetSmallFont(), FString::Printf(TEXT("1. APPLE - 3 COINS   +1 HP   [%s]"), *AppleStatus), PanelX + 54.f, PanelY + 210.f, 1.50f, 1.50f);

        Canvas->SetDrawColor(GI->CanBuyStar() ? FColor::Cyan : FColor::Silver);
        Canvas->DrawText(GEngine->GetSmallFont(), FString::Printf(TEXT("2. STAR - 10 COINS   BLOCKS NEXT 2 HITS   [%s]"), *StarStatus), PanelX + 54.f, PanelY + 285.f, 1.50f, 1.50f);

        Canvas->SetDrawColor(GI->CanBuyArmour() ? FColor::Yellow : FColor::Silver);
        Canvas->DrawText(GEngine->GetSmallFont(), FString::Printf(TEXT("3. ARMOUR - 25 COINS   +10 MAX HP & +10 HP   [%s]"), *ArmourStatus), PanelX + 54.f, PanelY + 360.f, 1.50f, 1.50f);

        Canvas->SetDrawColor(FColor::White);
        Canvas->DrawText(GEngine->GetSmallFont(), TEXT("Press 1, 2 or 3 to buy. Press 4 or ENTER to continue."), PanelX + 36.f, PanelY + 455.f, 1.35f, 1.35f);
        DrawFloatingMessages();
        return;
    }

    if (GM->bStageClear)
    {
        DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.78f), ScreenW * 0.18f, ScreenH * 0.28f, ScreenW * 0.64f, 220.f);
        Canvas->SetDrawColor(FColor::Yellow);
        Canvas->DrawText(GEngine->GetLargeFont(), TEXT("STAGE 1 COMPLETE"), ScreenW * 0.34f, ScreenH * 0.33f, 1.55f, 1.55f);
        Canvas->SetDrawColor(FColor::White);
        Canvas->DrawText(GEngine->GetSmallFont(), TEXT("Stage 2 - Sunny Meadow will be added next."), ScreenW * 0.28f, ScreenH * 0.46f, 1.40f, 1.40f);
        DrawFloatingMessages();
        return;
    }

    if (!GM->bBattleActive)
    {
        DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.50f), ScreenW * 0.25f, ScreenH - 88.f, ScreenW * 0.50f, 58.f);
        Canvas->SetDrawColor(FColor::White);
        Canvas->DrawText(GEngine->GetSmallFont(), TEXT("A / D or Arrow Keys: Move     Space: Jump"), ScreenW * 0.30f, ScreenH - 73.f, 1.30f, 1.30f);
        DrawFloatingMessages();
        return;
    }

    const float PanelX = ScreenW * 0.14f;
    const float PanelY = ScreenH * 0.42f;
    const float PanelW = ScreenW * 0.72f;
    const float PanelH = 390.f;

    DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.84f), PanelX, PanelY, PanelW, PanelH);

    const bool bBossBattle = GM->CurrentEnemy && GM->CurrentEnemy->bBoss;
    const FString BattleTitle = bBossBattle ? TEXT("BOSS BATTLE") : TEXT("WORD BATTLE");

    Canvas->SetDrawColor(FColor::Yellow);
    Canvas->DrawText(GEngine->GetLargeFont(), BattleTitle, PanelX + 32.f, PanelY + 20.f, 1.35f, 1.35f);

    Canvas->SetDrawColor(FColor::White);
    Canvas->DrawText(GEngine->GetSmallFont(), GM->CurrentQuestion.Prompt, PanelX + 32.f, PanelY + 88.f, 1.55f, 1.55f);

    for (int32 i = 0; i < GM->CurrentQuestion.Answers.Num() && i < 4; ++i)
    {
        const float Y = PanelY + 150.f + i * 48.f;
        Canvas->DrawText(GEngine->GetSmallFont(), FString::Printf(TEXT("%d. %s"), i + 1, *GM->CurrentQuestion.Answers[i]), PanelX + 58.f, Y, 1.40f, 1.40f);
    }

    Canvas->SetDrawColor(FColor::Green);
    Canvas->DrawText(GEngine->GetSmallFont(), TEXT("Press 1, 2, 3 or 4 to answer"), PanelX + 32.f, PanelY + 345.f, 1.30f, 1.30f);

    DrawFloatingMessages();
}

#include "WordQuestHUD.h"
#include "WordQuestGameMode.h"
#include "WordQuestGameInstance.h"
#include "WordQuestEnemy.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

void AWordQuestHUD::DrawHUD()
{
    Super::DrawHUD();

    if (!Canvas || !GEngine) return;

    AWordQuestGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<AWordQuestGameMode>() : nullptr;
    UWordQuestGameInstance* GI = GetGameInstance<UWordQuestGameInstance>();
    if (!GM || !GI) return;

    const float ScreenW = Canvas->SizeX;
    const float ScreenH = Canvas->SizeY;

    DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.55f), 20.f, 20.f, 460.f, 92.f);
    Canvas->SetDrawColor(FColor::White);
    Canvas->DrawText(GEngine->GetSmallFont(), FString::Printf(TEXT("STAGE 1 - WHISPERING FOREST    WAVE %d/5"), GM->CurrentWave), 35.f, 32.f, 1.1f, 1.1f);
    Canvas->DrawText(GEngine->GetSmallFont(), FString::Printf(TEXT("HP: %d/%d    COINS: %d    DAMAGE: %d"), GI->PlayerState.CurrentHP, GI->PlayerState.MaxHP, GI->PlayerState.Coins, GI->PlayerState.Damage), 35.f, 62.f, 1.1f, 1.1f);
    Canvas->DrawText(GEngine->GetSmallFont(), GI->PlayerState.bHasSword ? TEXT("SWORD: COLLECTED") : TEXT("SWORD: FIND IT AHEAD"), 35.f, 88.f, 1.0f, 1.0f);

    if (GM->bStageClear)
    {
        DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.75f), ScreenW * 0.20f, ScreenH * 0.30f, ScreenW * 0.60f, 180.f);
        Canvas->SetDrawColor(FColor::Yellow);
        Canvas->DrawText(GEngine->GetLargeFont(), TEXT("STAGE 1 CLEAR!"), ScreenW * 0.37f, ScreenH * 0.34f, 1.25f, 1.25f);
        Canvas->SetDrawColor(FColor::White);
        Canvas->DrawText(GEngine->GetSmallFont(), TEXT("The shop and Stage 2 transition are coming next."), ScreenW * 0.31f, ScreenH * 0.45f, 1.15f, 1.15f);
        return;
    }

    if (!GM->bBattleActive)
    {
        DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.45f), ScreenW * 0.32f, ScreenH - 70.f, ScreenW * 0.36f, 45.f);
        Canvas->SetDrawColor(FColor::White);
        Canvas->DrawText(GEngine->GetSmallFont(), TEXT("A / D or Arrow Keys: Move     Space: Jump"), ScreenW * 0.35f, ScreenH - 58.f, 1.0f, 1.0f);
        return;
    }

    const float PanelX = ScreenW * 0.18f;
    const float PanelY = ScreenH * 0.48f;
    const float PanelW = ScreenW * 0.64f;
    const float PanelH = 300.f;

    DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.82f), PanelX, PanelY, PanelW, PanelH);

    const bool bBossBattle = GM->CurrentEnemy && GM->CurrentEnemy->bBoss;
    const FString BattleTitle = bBossBattle ? TEXT("BOSS BATTLE") : TEXT("WORD BATTLE");

    Canvas->SetDrawColor(FColor::Yellow);
    Canvas->DrawText(GEngine->GetLargeFont(), BattleTitle, PanelX + 28.f, PanelY + 22.f, 1.0f, 1.0f);

    Canvas->SetDrawColor(FColor::White);
    Canvas->DrawText(GEngine->GetSmallFont(), GM->CurrentQuestion.Prompt, PanelX + 28.f, PanelY + 72.f, 1.25f, 1.25f);

    for (int32 i = 0; i < GM->CurrentQuestion.Answers.Num() && i < 4; ++i)
    {
        const float Y = PanelY + 120.f + i * 38.f;
        Canvas->DrawText(GEngine->GetSmallFont(), FString::Printf(TEXT("%d. %s"), i + 1, *GM->CurrentQuestion.Answers[i]), PanelX + 48.f, Y, 1.15f, 1.15f);
    }

    Canvas->SetDrawColor(FColor::Green);
    Canvas->DrawText(GEngine->GetSmallFont(), TEXT("Press 1, 2, 3 or 4 to answer"), PanelX + 28.f, PanelY + 270.f, 1.05f, 1.05f);
}

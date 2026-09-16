#include "WordQuestMenuHUD.h"
#include "WordQuestGameMode.h"
#include "Engine/Canvas.h"
#include "Engine/World.h"

void AWordQuestMenuHUD::DrawHUD()
{
    Super::DrawHUD();

    if (!Canvas || !GetWorld()) return;

    AWordQuestGameMode* GM = GetWorld()->GetAuthGameMode<AWordQuestGameMode>();
    if (!GM || !GM->bMainMenuOpen) return;

    const float ScreenW = Canvas->SizeX;
    const float ScreenH = Canvas->SizeY;

    // These bounds match the generated two-button title screen artwork.
    const bool bStartSelected = GM->MainMenuSelection == 0;

    const float StartX = ScreenW * 0.365f;
    const float StartY = ScreenH * 0.495f;
    const float StartW = ScreenW * 0.305f;
    const float StartH = ScreenH * 0.120f;

    const float ExitX = ScreenW * 0.400f;
    const float ExitY = ScreenH * 0.625f;
    const float ExitW = ScreenW * 0.220f;
    const float ExitH = ScreenH * 0.095f;

    const float X = bStartSelected ? StartX : ExitX;
    const float Y = bStartSelected ? StartY : ExitY;
    const float W = bStartSelected ? StartW : ExitW;
    const float H = bStartSelected ? StartH : ExitH;

    const FLinearColor Glow(1.f, 0.82f, 0.18f, 0.78f);
    const float T = FMath::Max(3.f, ScreenH * 0.004f);

    DrawRect(Glow, X, Y, W, T);
    DrawRect(Glow, X, Y + H - T, W, T);
    DrawRect(Glow, X, Y, T, H);
    DrawRect(Glow, X + W - T, Y, T, H);
}

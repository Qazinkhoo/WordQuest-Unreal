#include "WordQuestMenuHUD.h"
#include "WordQuestGameMode.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "Engine/World.h"

void AWordQuestMenuHUD::DrawHUD()
{
    // The base HUD draws the complete baked intro artwork.
    Super::DrawHUD();

    if (!Canvas || !GetWorld()) return;

    AWordQuestGameMode* GM = GetWorld()->GetAuthGameMode<AWordQuestGameMode>();
    if (!GM || !GM->bMainMenuOpen) return;

    // No selection rectangle is drawn here. Only the creator credit is added.
    UFont* Font = GetHUDTextFont();
    if (!Font) return;

    const FString Credit = TEXT("MADE BY MR QAZIN KHOO");

    // Exactly twice the previous size (0.62 -> 1.24).
    const float Scale = 1.24f;

    float TextW = 0.f;
    float TextH = 0.f;
    Canvas->StrLen(Font, Credit, TextW, TextH);

    const float X = (Canvas->SizeX - TextW * Scale) * 0.5f;

    // Move it slightly upward so the larger credit remains fully visible.
    const float Y = Canvas->SizeY * 0.82f;

    Canvas->SetDrawColor(FColor(255, 245, 210));
    Canvas->DrawText(Font, Credit, X, Y, Scale, Scale);
}

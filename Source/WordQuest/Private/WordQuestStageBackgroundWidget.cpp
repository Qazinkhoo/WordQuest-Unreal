#include "WordQuestStageBackgroundWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateBrush.h"
#include "Widgets/SNullWidget.h"

namespace
{
    constexpr float BackgroundCanvasWidth = 7680.f;
    constexpr float BackgroundCanvasHeight = 1440.f;
}

TSharedRef<SWidget> UWordQuestStageBackgroundWidget::RebuildWidget()
{
    TileCanvas = NewObject<UCanvasPanel>(this, TEXT("StageBackgroundTileCanvas"));
    if (!TileCanvas)
    {
        return SNullWidget::NullWidget;
    }

    return TileCanvas->TakeWidget();
}

void UWordQuestStageBackgroundWidget::NativeConstruct()
{
    Super::NativeConstruct();
    RefreshBackground();
}

void UWordQuestStageBackgroundWidget::SetStageNumber(int32 InStageNumber)
{
    StageNumber = FMath::Clamp(InStageNumber, 1, 3);
    RefreshBackground();
}

void UWordQuestStageBackgroundWidget::RefreshBackground()
{
    if (!TileCanvas) return;

    const TCHAR* TexturePath = TEXT("/Game/UI/StageBackgrounds/Stage01_WhisperingForest.Stage01_WhisperingForest");

    switch (StageNumber)
    {
    case 1:
        TexturePath = TEXT("/Game/UI/StageBackgrounds/Stage01_WhisperingForest.Stage01_WhisperingForest");
        break;
    case 2:
        TexturePath = TEXT("/Game/UI/StageBackgrounds/Stage02_SunnyMeadow.Stage02_SunnyMeadow");
        break;
    case 3:
    default:
        TexturePath = TEXT("/Game/UI/StageBackgrounds/Stage03_CrystalCave.Stage03_CrystalCave");
        break;
    }

    BackgroundTexture = LoadObject<UTexture2D>(nullptr, TexturePath);
    TileCanvas->ClearChildren();
    BackgroundTiles.Reset();

    if (!BackgroundTexture)
    {
        SetVisibility(ESlateVisibility::Collapsed);
        return;
    }

    SetVisibility(ESlateVisibility::Visible);

    const float TextureWidth = FMath::Max(1.f, static_cast<float>(BackgroundTexture->GetSizeX()));
    const float TextureHeight = FMath::Max(1.f, static_cast<float>(BackgroundTexture->GetSizeY()));

    // Preserve the source image's natural aspect ratio. The artwork is portrait,
    // so instead of stretching one copy across the whole level we repeat several
    // correctly proportioned copies across the side-scrolling stage.
    const float TileHeight = BackgroundCanvasHeight;
    const float TileWidth = TileHeight * (TextureWidth / TextureHeight);
    const int32 TileCount = FMath::CeilToInt(BackgroundCanvasWidth / TileWidth) + 2;

    for (int32 Index = 0; Index < TileCount; ++Index)
    {
        const FName TileName(*FString::Printf(TEXT("StageBackgroundTile_%02d"), Index));
        UImage* TileImage = NewObject<UImage>(this, TileName);
        if (!TileImage) continue;

        FSlateBrush Brush;
        Brush.SetResourceObject(BackgroundTexture);
        Brush.DrawAs = ESlateBrushDrawType::Image;
        Brush.ImageSize = FVector2D(TextureWidth, TextureHeight);
        Brush.Mirroring = (Index % 2 == 0)
            ? ESlateBrushMirrorType::NoMirror
            : ESlateBrushMirrorType::Horizontal;

        TileImage->SetBrush(Brush);
        TileImage->SetColorAndOpacity(FLinearColor::White);

        UCanvasPanelSlot* CanvasSlot = TileCanvas->AddChildToCanvas(TileImage);
        if (CanvasSlot)
        {
            CanvasSlot->SetAnchors(FAnchors(0.f, 0.f, 0.f, 0.f));
            CanvasSlot->SetAlignment(FVector2D(0.f, 0.f));
            CanvasSlot->SetPosition(FVector2D(Index * TileWidth, 0.f));
            CanvasSlot->SetSize(FVector2D(TileWidth, TileHeight));
            CanvasSlot->SetAutoSize(false);
        }

        BackgroundTiles.Add(TileImage);
    }
}

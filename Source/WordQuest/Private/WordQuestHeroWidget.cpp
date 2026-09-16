#include "WordQuestHeroWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateBrush.h"
#include "Widgets/SNullWidget.h"

TSharedRef<SWidget> UWordQuestHeroWidget::RebuildWidget()
{
    RootPanel = NewObject<UCanvasPanel>(this, TEXT("HeroRoot"));
    HeadImage = NewObject<UImage>(this, TEXT("HeroHeadBand"));
    TorsoImage = NewObject<UImage>(this, TEXT("HeroTorsoBand"));
    LegsImage = NewObject<UImage>(this, TEXT("HeroLegsBand"));

    if (!RootPanel || !HeadImage || !TorsoImage || !LegsImage)
    {
        return SNullWidget::NullWidget;
    }

    UCanvasPanelSlot* HeadSlot = RootPanel->AddChildToCanvas(HeadImage);
    UCanvasPanelSlot* TorsoSlot = RootPanel->AddChildToCanvas(TorsoImage);
    UCanvasPanelSlot* LegsSlot = RootPanel->AddChildToCanvas(LegsImage);

    if (HeadSlot)
    {
        HeadSlot->SetPosition(FVector2D(0.f, 0.f));
        HeadSlot->SetSize(FVector2D(100.f, 102.f));
    }
    if (TorsoSlot)
    {
        TorsoSlot->SetPosition(FVector2D(0.f, 102.f));
        TorsoSlot->SetSize(FVector2D(100.f, 102.f));
    }
    if (LegsSlot)
    {
        LegsSlot->SetPosition(FVector2D(0.f, 204.f));
        LegsSlot->SetSize(FVector2D(100.f, 96.f));
    }

    HeadImage->SetRenderTransformPivot(FVector2D(0.5f, 1.f));
    TorsoImage->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
    LegsImage->SetRenderTransformPivot(FVector2D(0.5f, 0.f));

    return RootPanel->TakeWidget();
}

void UWordQuestHeroWidget::NativeConstruct()
{
    Super::NativeConstruct();

    HeroTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/WQHeroSheet.WQHeroSheet"));
    RefreshBrushes();
    ResetPartTransforms();
}

void UWordQuestHeroWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!HeadImage || !TorsoImage || !LegsImage) return;

    if (!bMoving && !bAirborne)
    {
        WalkPhase = 0.f;
        ResetPartTransforms();
        return;
    }

    if (bMoving)
    {
        WalkPhase += InDeltaTime * 10.5f;

        const float Swing = FMath::Sin(WalkPhase);
        const float OppositeSwing = FMath::Sin(WalkPhase + PI);
        const float StepLift = FMath::Abs(FMath::Sin(WalkPhase * 2.f));

        FWidgetTransform HeadTransform;
        HeadTransform.Translation = FVector2D(Swing * 0.6f, -StepLift * 0.7f);
        HeadTransform.Scale = FVector2D(1.f, 1.f);
        HeadTransform.Angle = Swing * 0.8f;
        HeadImage->SetRenderTransform(HeadTransform);

        FWidgetTransform TorsoTransform;
        TorsoTransform.Translation = FVector2D(Swing * 1.8f, -StepLift * 1.2f);
        TorsoTransform.Scale = FVector2D(1.f, 1.f);
        TorsoTransform.Angle = Swing * 2.0f;
        TorsoImage->SetRenderTransform(TorsoTransform);

        FWidgetTransform LegsTransform;
        LegsTransform.Translation = FVector2D(OppositeSwing * 2.6f, StepLift * 0.5f);
        LegsTransform.Scale = FVector2D(1.f + StepLift * 0.012f, 1.f - StepLift * 0.018f);
        LegsTransform.Angle = OppositeSwing * 2.8f;
        LegsImage->SetRenderTransform(LegsTransform);
    }
    else if (bAirborne)
    {
        FWidgetTransform HeadTransform;
        HeadTransform.Scale = FVector2D(1.f, 1.f);
        HeadTransform.Translation = FVector2D(0.f, -1.f);
        HeadImage->SetRenderTransform(HeadTransform);

        FWidgetTransform TorsoTransform;
        TorsoTransform.Scale = FVector2D(1.f, 1.f);
        TorsoTransform.Angle = bFacingLeft ? 1.5f : -1.5f;
        TorsoImage->SetRenderTransform(TorsoTransform);

        FWidgetTransform LegsTransform;
        LegsTransform.Scale = FVector2D(1.03f, 0.96f);
        LegsTransform.Translation = FVector2D(0.f, -2.f);
        LegsTransform.Angle = bFacingLeft ? -3.f : 3.f;
        LegsImage->SetRenderTransform(LegsTransform);
    }
}

void UWordQuestHeroWidget::SetFacingLeft(bool bInFacingLeft)
{
    if (bFacingLeft == bInFacingLeft) return;

    bFacingLeft = bInFacingLeft;
    RefreshBrushes();
}

void UWordQuestHeroWidget::SetMovementState(bool bInMoving, bool bInAir)
{
    bMoving = bInMoving;
    bAirborne = bInAir;
}

void UWordQuestHeroWidget::ApplyBandBrush(UImage* Image, const FBox2f& UVRegion, const FVector2D& ImageSize)
{
    if (!Image || !HeroTexture) return;

    FSlateBrush Brush;
    Brush.SetResourceObject(HeroTexture);
    Brush.DrawAs = ESlateBrushDrawType::Image;
    Brush.ImageSize = ImageSize;
    Brush.SetUVRegion(UVRegion);

    Image->SetBrush(Brush);
    Image->SetColorAndOpacity(FLinearColor::White);
}

void UWordQuestHeroWidget::RefreshBrushes()
{
    if (!HeroTexture || !HeadImage || !TorsoImage || !LegsImage) return;

    const float MinX = bFacingLeft ? 0.297652f : 0.791436f;
    const float MaxX = bFacingLeft ? 0.475138f : 0.968923f;
    const float MinY = 0.151013f;
    const float MaxY = 0.863720f;
    const float Height = MaxY - MinY;

    const float HeadEndY = MinY + Height * 0.34f;
    const float TorsoEndY = MinY + Height * 0.68f;

    ApplyBandBrush(HeadImage,
        FBox2f(FVector2f(MinX, MinY), FVector2f(MaxX, HeadEndY)),
        FVector2D(100.f, 102.f));

    ApplyBandBrush(TorsoImage,
        FBox2f(FVector2f(MinX, HeadEndY), FVector2f(MaxX, TorsoEndY)),
        FVector2D(100.f, 102.f));

    ApplyBandBrush(LegsImage,
        FBox2f(FVector2f(MinX, TorsoEndY), FVector2f(MaxX, MaxY)),
        FVector2D(100.f, 96.f));
}

void UWordQuestHeroWidget::ResetPartTransforms()
{
    FWidgetTransform Identity;
    Identity.Scale = FVector2D(1.f, 1.f);

    if (HeadImage) HeadImage->SetRenderTransform(Identity);
    if (TorsoImage) TorsoImage->SetRenderTransform(Identity);
    if (LegsImage) LegsImage->SetRenderTransform(Identity);
}

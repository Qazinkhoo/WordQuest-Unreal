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
    HeadImage = NewObject<UImage>(this, TEXT("HeroHead"));
    TorsoImage = NewObject<UImage>(this, TEXT("HeroTorso"));
    LeftArmImage = NewObject<UImage>(this, TEXT("HeroLeftArm"));
    RightArmImage = NewObject<UImage>(this, TEXT("HeroRightArm"));
    LeftLegImage = NewObject<UImage>(this, TEXT("HeroLeftLeg"));
    RightLegImage = NewObject<UImage>(this, TEXT("HeroRightLeg"));

    if (!RootPanel || !HeadImage || !TorsoImage || !LeftArmImage || !RightArmImage || !LeftLegImage || !RightLegImage)
    {
        return SNullWidget::NullWidget;
    }

    // The layout below is taken directly from the assembled reference preview.
    // Back limbs are drawn first, torso in the middle, then front limbs and head.
    UCanvasPanelSlot* LeftLegSlot = RootPanel->AddChildToCanvas(LeftLegImage);
    UCanvasPanelSlot* LeftArmSlot = RootPanel->AddChildToCanvas(LeftArmImage);
    UCanvasPanelSlot* TorsoSlot = RootPanel->AddChildToCanvas(TorsoImage);
    UCanvasPanelSlot* RightLegSlot = RootPanel->AddChildToCanvas(RightLegImage);
    UCanvasPanelSlot* RightArmSlot = RootPanel->AddChildToCanvas(RightArmImage);
    UCanvasPanelSlot* HeadSlot = RootPanel->AddChildToCanvas(HeadImage);

    if (HeadSlot)
    {
        HeadSlot->SetPosition(FVector2D(31.f, 3.f));
        HeadSlot->SetSize(FVector2D(76.f, 64.f));
        HeadSlot->SetZOrder(6);
    }
    if (TorsoSlot)
    {
        TorsoSlot->SetPosition(FVector2D(25.f, 62.f));
        TorsoSlot->SetSize(FVector2D(70.f, 72.f));
        TorsoSlot->SetZOrder(3);
    }
    if (LeftArmSlot)
    {
        LeftArmSlot->SetPosition(FVector2D(18.f, 75.f));
        LeftArmSlot->SetSize(FVector2D(33.f, 72.f));
        LeftArmSlot->SetZOrder(2);
    }
    if (RightArmSlot)
    {
        RightArmSlot->SetPosition(FVector2D(58.f, 78.f));
        RightArmSlot->SetSize(FVector2D(45.f, 72.f));
        RightArmSlot->SetZOrder(5);
    }
    if (LeftLegSlot)
    {
        LeftLegSlot->SetPosition(FVector2D(25.f, 132.f));
        LeftLegSlot->SetSize(FVector2D(51.f, 78.f));
        LeftLegSlot->SetZOrder(1);
    }
    if (RightLegSlot)
    {
        RightLegSlot->SetPosition(FVector2D(48.f, 132.f));
        RightLegSlot->SetSize(FVector2D(54.f, 78.f));
        RightLegSlot->SetZOrder(4);
    }

    // Rotate limbs from the shoulder/hip so they remain attached while moving.
    HeadImage->SetRenderTransformPivot(FVector2D(0.5f, 0.92f));
    TorsoImage->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
    LeftArmImage->SetRenderTransformPivot(FVector2D(0.58f, 0.08f));
    RightArmImage->SetRenderTransformPivot(FVector2D(0.42f, 0.08f));
    LeftLegImage->SetRenderTransformPivot(FVector2D(0.58f, 0.04f));
    RightLegImage->SetRenderTransformPivot(FVector2D(0.42f, 0.04f));

    RootPanel->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
    return RootPanel->TakeWidget();
}

void UWordQuestHeroWidget::NativeConstruct()
{
    Super::NativeConstruct();

    AtlasTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/WQHeroPartsAtlas.WQHeroPartsAtlas"));
    if (AtlasTexture)
    {
        AtlasTexture->Filter = TF_Nearest;
        AtlasTexture->UpdateResource();
    }

    RefreshBrushes();
    ResetPartTransforms();
    SetFacingLeft(false);
}

void UWordQuestHeroWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!HeadImage || !TorsoImage || !LeftArmImage || !RightArmImage || !LeftLegImage || !RightLegImage)
    {
        return;
    }

    if (!bMoving && !bAirborne)
    {
        WalkPhase += InDeltaTime * 1.8f;
        ResetPartTransforms();

        const float IdleBob = FMath::Sin(WalkPhase) * 0.35f;
        FWidgetTransform HeadTransform;
        HeadTransform.Scale = FVector2D(1.f, 1.f);
        HeadTransform.Translation = FVector2D(0.f, IdleBob);
        HeadImage->SetRenderTransform(HeadTransform);
        return;
    }

    if (bAirborne)
    {
        ResetPartTransforms();

        FWidgetTransform HeadTransform;
        HeadTransform.Scale = FVector2D(1.f, 1.f);
        HeadTransform.Translation = FVector2D(0.f, -1.5f);
        HeadImage->SetRenderTransform(HeadTransform);

        FWidgetTransform TorsoTransform;
        TorsoTransform.Scale = FVector2D(1.f, 1.f);
        TorsoTransform.Translation = FVector2D(0.f, -1.5f);
        TorsoImage->SetRenderTransform(TorsoTransform);

        FWidgetTransform LeftArmTransform;
        LeftArmTransform.Scale = FVector2D(1.f, 1.f);
        LeftArmTransform.Angle = 11.f;
        LeftArmImage->SetRenderTransform(LeftArmTransform);

        FWidgetTransform RightArmTransform;
        RightArmTransform.Scale = FVector2D(1.f, 1.f);
        RightArmTransform.Angle = -12.f;
        RightArmImage->SetRenderTransform(RightArmTransform);

        FWidgetTransform LeftLegTransform;
        LeftLegTransform.Scale = FVector2D(1.f, 1.f);
        LeftLegTransform.Angle = -7.f;
        LeftLegTransform.Translation = FVector2D(0.f, -2.f);
        LeftLegImage->SetRenderTransform(LeftLegTransform);

        FWidgetTransform RightLegTransform;
        RightLegTransform.Scale = FVector2D(1.f, 1.f);
        RightLegTransform.Angle = 8.f;
        RightLegTransform.Translation = FVector2D(0.f, -3.f);
        RightLegImage->SetRenderTransform(RightLegTransform);
        return;
    }

    WalkPhase += InDeltaTime * 8.f;
    const float S = FMath::Sin(WalkPhase);
    const float Lift = FMath::Abs(FMath::Sin(WalkPhase * 2.f));

    FWidgetTransform HeadTransform;
    HeadTransform.Scale = FVector2D(1.f, 1.f);
    HeadTransform.Translation = FVector2D(0.f, -Lift * 0.45f);
    HeadTransform.Angle = S * 0.35f;
    HeadImage->SetRenderTransform(HeadTransform);

    FWidgetTransform TorsoTransform;
    TorsoTransform.Scale = FVector2D(1.f, 1.f);
    TorsoTransform.Translation = FVector2D(0.f, -Lift * 0.75f);
    TorsoTransform.Angle = S * 0.55f;
    TorsoImage->SetRenderTransform(TorsoTransform);

    FWidgetTransform LeftArmTransform;
    LeftArmTransform.Scale = FVector2D(1.f, 1.f);
    LeftArmTransform.Angle = S * 11.f;
    LeftArmImage->SetRenderTransform(LeftArmTransform);

    FWidgetTransform RightArmTransform;
    RightArmTransform.Scale = FVector2D(1.f, 1.f);
    RightArmTransform.Angle = -S * 11.f;
    RightArmImage->SetRenderTransform(RightArmTransform);

    FWidgetTransform LeftLegTransform;
    LeftLegTransform.Scale = FVector2D(1.f, 1.f);
    LeftLegTransform.Angle = -S * 7.f;
    LeftLegTransform.Translation = FVector2D(0.f, -FMath::Max(0.f, S) * 1.5f);
    LeftLegImage->SetRenderTransform(LeftLegTransform);

    FWidgetTransform RightLegTransform;
    RightLegTransform.Scale = FVector2D(1.f, 1.f);
    RightLegTransform.Angle = S * 7.f;
    RightLegTransform.Translation = FVector2D(0.f, -FMath::Max(0.f, -S) * 1.5f);
    RightLegImage->SetRenderTransform(RightLegTransform);
}

void UWordQuestHeroWidget::SetFacingLeft(bool bInFacingLeft)
{
    bFacingLeft = bInFacingLeft;

    if (RootPanel)
    {
        RootPanel->SetRenderScale(FVector2D(bFacingLeft ? -1.f : 1.f, 1.f));
    }
}

void UWordQuestHeroWidget::SetMovementState(bool bInMoving, bool bInAir)
{
    bMoving = bInMoving;
    bAirborne = bInAir;
}

void UWordQuestHeroWidget::ApplyPartBrush(UImage* Image, const FBox2f& UVRegion, const FVector2D& ImageSize)
{
    if (!Image || !AtlasTexture) return;

    FSlateBrush Brush;
    Brush.SetResourceObject(AtlasTexture);
    Brush.DrawAs = ESlateBrushDrawType::Image;
    Brush.ImageSize = ImageSize;
    Brush.SetUVRegion(UVRegion);

    Image->SetBrush(Brush);
    Image->SetColorAndOpacity(FLinearColor::White);
}

void UWordQuestHeroWidget::RefreshBrushes()
{
    if (!AtlasTexture) return;

    ApplyPartBrush(HeadImage,
        FBox2f(FVector2f(0.015625f, 0.020833f), FVector2f(0.300781f, 0.341146f)),
        FVector2D(76.f, 64.f));

    ApplyPartBrush(TorsoImage,
        FBox2f(FVector2f(0.332031f, 0.020833f), FVector2f(0.583984f, 0.364583f)),
        FVector2D(70.f, 72.f));

    ApplyPartBrush(LeftArmImage,
        FBox2f(FVector2f(0.625000f, 0.020833f), FVector2f(0.757813f, 0.403646f)),
        FVector2D(33.f, 72.f));

    ApplyPartBrush(RightArmImage,
        FBox2f(FVector2f(0.789063f, 0.020833f), FVector2f(0.968750f, 0.403646f)),
        FVector2D(45.f, 72.f));

    ApplyPartBrush(LeftLegImage,
        FBox2f(FVector2f(0.015625f, 0.468750f), FVector2f(0.177734f, 0.799479f)),
        FVector2D(51.f, 78.f));

    ApplyPartBrush(RightLegImage,
        FBox2f(FVector2f(0.214844f, 0.468750f), FVector2f(0.394531f, 0.809896f)),
        FVector2D(54.f, 78.f));
}

void UWordQuestHeroWidget::ResetPartTransforms()
{
    FWidgetTransform Identity;
    Identity.Scale = FVector2D(1.f, 1.f);

    if (HeadImage) HeadImage->SetRenderTransform(Identity);
    if (TorsoImage) TorsoImage->SetRenderTransform(Identity);
    if (LeftArmImage) LeftArmImage->SetRenderTransform(Identity);
    if (RightArmImage) RightArmImage->SetRenderTransform(Identity);
    if (LeftLegImage) LeftLegImage->SetRenderTransform(Identity);
    if (RightLegImage) RightLegImage->SetRenderTransform(Identity);
}

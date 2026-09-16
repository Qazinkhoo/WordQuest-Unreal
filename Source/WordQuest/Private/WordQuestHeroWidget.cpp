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

    // Back limbs first, then torso/front limbs/head for cleaner layering.
    UCanvasPanelSlot* LeftLegSlot = RootPanel->AddChildToCanvas(LeftLegImage);
    UCanvasPanelSlot* LeftArmSlot = RootPanel->AddChildToCanvas(LeftArmImage);
    UCanvasPanelSlot* TorsoSlot = RootPanel->AddChildToCanvas(TorsoImage);
    UCanvasPanelSlot* RightLegSlot = RootPanel->AddChildToCanvas(RightLegImage);
    UCanvasPanelSlot* RightArmSlot = RootPanel->AddChildToCanvas(RightArmImage);
    UCanvasPanelSlot* HeadSlot = RootPanel->AddChildToCanvas(HeadImage);

    if (HeadSlot)
    {
        HeadSlot->SetPosition(FVector2D(24.f, 0.f));
        HeadSlot->SetSize(FVector2D(82.f, 69.f));
        HeadSlot->SetZOrder(6);
    }
    if (TorsoSlot)
    {
        TorsoSlot->SetPosition(FVector2D(30.f, 61.f));
        TorsoSlot->SetSize(FVector2D(72.f, 74.f));
        TorsoSlot->SetZOrder(3);
    }
    if (LeftArmSlot)
    {
        LeftArmSlot->SetPosition(FVector2D(25.f, 73.f));
        LeftArmSlot->SetSize(FVector2D(29.f, 63.f));
        LeftArmSlot->SetZOrder(2);
    }
    if (RightArmSlot)
    {
        RightArmSlot->SetPosition(FVector2D(76.f, 75.f));
        RightArmSlot->SetSize(FVector2D(40.f, 64.f));
        RightArmSlot->SetZOrder(5);
    }
    if (LeftLegSlot)
    {
        LeftLegSlot->SetPosition(FVector2D(42.f, 126.f));
        LeftLegSlot->SetSize(FVector2D(42.f, 65.f));
        LeftLegSlot->SetZOrder(1);
    }
    if (RightLegSlot)
    {
        RightLegSlot->SetPosition(FVector2D(57.f, 126.f));
        RightLegSlot->SetSize(FVector2D(46.f, 67.f));
        RightLegSlot->SetZOrder(4);
    }

    HeadImage->SetRenderTransformPivot(FVector2D(0.5f, 0.9f));
    TorsoImage->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
    LeftArmImage->SetRenderTransformPivot(FVector2D(0.5f, 0.08f));
    RightArmImage->SetRenderTransformPivot(FVector2D(0.5f, 0.08f));
    LeftLegImage->SetRenderTransformPivot(FVector2D(0.5f, 0.06f));
    RightLegImage->SetRenderTransformPivot(FVector2D(0.5f, 0.06f));

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
        WalkPhase += InDeltaTime * 2.f;
        ResetPartTransforms();

        const float IdleBob = FMath::Sin(WalkPhase) * 0.6f;
        FWidgetTransform HeadTransform;
        HeadTransform.Scale = FVector2D(1.f, 1.f);
        HeadTransform.Translation = FVector2D(0.f, IdleBob);
        HeadImage->SetRenderTransform(HeadTransform);
        return;
    }

    if (bAirborne)
    {
        FWidgetTransform HeadTransform;
        HeadTransform.Scale = FVector2D(1.f, 1.f);
        HeadTransform.Translation = FVector2D(0.f, -2.f);
        HeadImage->SetRenderTransform(HeadTransform);

        FWidgetTransform TorsoTransform;
        TorsoTransform.Scale = FVector2D(1.f, 1.f);
        TorsoTransform.Translation = FVector2D(0.f, -2.f);
        TorsoTransform.Angle = -1.5f;
        TorsoImage->SetRenderTransform(TorsoTransform);

        FWidgetTransform LeftArmTransform;
        LeftArmTransform.Scale = FVector2D(1.f, 1.f);
        LeftArmTransform.Angle = 25.f;
        LeftArmImage->SetRenderTransform(LeftArmTransform);

        FWidgetTransform RightArmTransform;
        RightArmTransform.Scale = FVector2D(1.f, 1.f);
        RightArmTransform.Angle = -28.f;
        RightArmImage->SetRenderTransform(RightArmTransform);

        FWidgetTransform LeftLegTransform;
        LeftLegTransform.Scale = FVector2D(1.f, 1.f);
        LeftLegTransform.Angle = -16.f;
        LeftLegTransform.Translation = FVector2D(0.f, -3.f);
        LeftLegImage->SetRenderTransform(LeftLegTransform);

        FWidgetTransform RightLegTransform;
        RightLegTransform.Scale = FVector2D(1.f, 1.f);
        RightLegTransform.Angle = 20.f;
        RightLegTransform.Translation = FVector2D(0.f, -5.f);
        RightLegImage->SetRenderTransform(RightLegTransform);
        return;
    }

    WalkPhase += InDeltaTime * 9.5f;
    const float S = FMath::Sin(WalkPhase);
    const float Lift = FMath::Abs(FMath::Sin(WalkPhase * 2.f));

    FWidgetTransform HeadTransform;
    HeadTransform.Scale = FVector2D(1.f, 1.f);
    HeadTransform.Translation = FVector2D(S * 0.4f, -Lift * 0.8f);
    HeadTransform.Angle = S * 0.7f;
    HeadImage->SetRenderTransform(HeadTransform);

    FWidgetTransform TorsoTransform;
    TorsoTransform.Scale = FVector2D(1.f, 1.f);
    TorsoTransform.Translation = FVector2D(S * 0.7f, -Lift * 1.2f);
    TorsoTransform.Angle = S * 1.5f;
    TorsoImage->SetRenderTransform(TorsoTransform);

    FWidgetTransform LeftArmTransform;
    LeftArmTransform.Scale = FVector2D(1.f, 1.f);
    LeftArmTransform.Angle = S * 19.f;
    LeftArmImage->SetRenderTransform(LeftArmTransform);

    FWidgetTransform RightArmTransform;
    RightArmTransform.Scale = FVector2D(1.f, 1.f);
    RightArmTransform.Angle = -S * 19.f;
    RightArmImage->SetRenderTransform(RightArmTransform);

    FWidgetTransform LeftLegTransform;
    LeftLegTransform.Scale = FVector2D(1.f, 1.f);
    LeftLegTransform.Angle = -S * 18.f;
    LeftLegTransform.Translation = FVector2D(0.f, -FMath::Max(0.f, S) * 2.5f);
    LeftLegImage->SetRenderTransform(LeftLegTransform);

    FWidgetTransform RightLegTransform;
    RightLegTransform.Scale = FVector2D(1.f, 1.f);
    RightLegTransform.Angle = S * 18.f;
    RightLegTransform.Translation = FVector2D(0.f, -FMath::Max(0.f, -S) * 2.5f);
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
        FVector2D(146.f, 123.f));

    ApplyPartBrush(TorsoImage,
        FBox2f(FVector2f(0.332031f, 0.020833f), FVector2f(0.583984f, 0.364583f)),
        FVector2D(129.f, 132.f));

    ApplyPartBrush(LeftArmImage,
        FBox2f(FVector2f(0.625000f, 0.020833f), FVector2f(0.757813f, 0.403646f)),
        FVector2D(68.f, 147.f));

    ApplyPartBrush(RightArmImage,
        FBox2f(FVector2f(0.789063f, 0.020833f), FVector2f(0.968750f, 0.403646f)),
        FVector2D(92.f, 147.f));

    ApplyPartBrush(LeftLegImage,
        FBox2f(FVector2f(0.015625f, 0.468750f), FVector2f(0.177734f, 0.799479f)),
        FVector2D(83.f, 127.f));

    ApplyPartBrush(RightLegImage,
        FBox2f(FVector2f(0.214844f, 0.468750f), FVector2f(0.394531f, 0.809896f)),
        FVector2D(92.f, 131.f));
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

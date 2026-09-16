#include "WordQuestShadowWidget.h"
#include "Styling/SlateColorBrush.h"
#include "Widgets/Images/SImage.h"

TSharedRef<SWidget> UWordQuestShadowWidget::RebuildWidget()
{
    static FSlateColorBrush ShadowBrush(FLinearColor::White);

    return SNew(SImage)
        .Image(&ShadowBrush)
        .ColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 0.42f));
}

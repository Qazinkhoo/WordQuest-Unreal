#include "WordQuestShadowWidget.h"
#include "Widgets/Layout/SBorder.h"

TSharedRef<SWidget> UWordQuestShadowWidget::RebuildWidget()
{
    return SNew(SBorder)
        .Padding(0.f)
        .BorderBackgroundColor(FLinearColor(0.f, 0.f, 0.f, 0.32f));
}

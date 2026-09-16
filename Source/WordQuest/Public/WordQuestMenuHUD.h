#pragma once

#include "CoreMinimal.h"
#include "WordQuestHUD.h"
#include "WordQuestMenuHUD.generated.h"

UCLASS()
class WORDQUEST_API AWordQuestMenuHUD : public AWordQuestHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;
};

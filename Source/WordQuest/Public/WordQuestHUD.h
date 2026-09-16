#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "WordQuestHUD.generated.h"

UCLASS()
class WORDQUEST_API AWordQuestHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;
};

#pragma once

#include "CoreMinimal.h"
#include "WordQuestTypes.generated.h"

USTRUCT(BlueprintType)
struct FWordQuestQuestion
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName Id;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Prompt;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FString> Answers;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 CorrectAnswerIndex = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Category;
};

USTRUCT(BlueprintType)
struct FWordQuestPlayerState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 CurrentHP = 10;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 MaxHP = 10;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Damage = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Coins = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 StarProtectionHits = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bHasSword = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Stage = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Wave = 1;
};

USTRUCT(BlueprintType)
struct FWordQuestStageDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 StageNumber = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString StageName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Theme;
};

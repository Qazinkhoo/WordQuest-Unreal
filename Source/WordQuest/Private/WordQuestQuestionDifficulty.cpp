#include "WordQuestQuestionSubsystem.h"

int32 UWordQuestQuestionSubsystem::GetDifficultyScore(const FWordQuestQuestion& Question) const
{
    const FString Category = Question.Category;

    if (Category == TEXT("Vocabulary") || Category == TEXT("Spelling") || Category == TEXT("Prepositions")) return 1;
    if (Category == TEXT("Grammar") || Category == TEXT("Word Classes")) return 2;
    if (Category == TEXT("Tenses") || Category == TEXT("Future Tense") || Category == TEXT("Comparatives") || Category == TEXT("Adverbs")) return 3;
    if (Category == TEXT("Conjunctions")) return 4;
    if (Category == TEXT("Punctuation") || Category == TEXT("Sentence Types")) return 5;

    return 3;
}

void UWordQuestQuestionSubsystem::ShuffleAnswers(FWordQuestQuestion& Question) const
{
    for (int32 i = Question.Answers.Num() - 1; i > 0; --i)
    {
        const int32 j = FMath::RandRange(0, i);
        if (i == j) continue;

        Question.Answers.Swap(i, j);
        if (Question.CorrectAnswerIndex == i) Question.CorrectAnswerIndex = j;
        else if (Question.CorrectAnswerIndex == j) Question.CorrectAnswerIndex = i;
    }
}

bool UWordQuestQuestionSubsystem::GetNextQuestionForWave(int32 Wave, FWordQuestQuestion& OutQuestion)
{
    if (RemainingQuestionIndices.IsEmpty()) return false;

    const int32 TargetDifficulty = FMath::Clamp(Wave, 1, 5);
    TArray<int32> CandidatePositions;

    for (int32 Position = 0; Position < RemainingQuestionIndices.Num(); ++Position)
    {
        const int32 BankIndex = RemainingQuestionIndices[Position];
        if (!QuestionBank.IsValidIndex(BankIndex)) continue;

        if (GetDifficultyScore(QuestionBank[BankIndex]) == TargetDifficulty)
        {
            CandidatePositions.Add(Position);
        }
    }

    if (CandidatePositions.IsEmpty())
    {
        int32 BestDistance = MAX_int32;
        for (int32 Position = 0; Position < RemainingQuestionIndices.Num(); ++Position)
        {
            const int32 BankIndex = RemainingQuestionIndices[Position];
            if (!QuestionBank.IsValidIndex(BankIndex)) continue;

            const int32 Distance = FMath::Abs(GetDifficultyScore(QuestionBank[BankIndex]) - TargetDifficulty);
            if (Distance < BestDistance)
            {
                CandidatePositions.Empty();
                BestDistance = Distance;
            }
            if (Distance == BestDistance)
            {
                CandidatePositions.Add(Position);
            }
        }
    }

    if (CandidatePositions.IsEmpty()) return false;

    const int32 Pick = CandidatePositions[FMath::RandRange(0, CandidatePositions.Num() - 1)];
    const int32 BankIndex = RemainingQuestionIndices[Pick];
    RemainingQuestionIndices.RemoveAt(Pick);

    OutQuestion = QuestionBank[BankIndex];
    ShuffleAnswers(OutQuestion);
    return true;
}

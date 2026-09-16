#include "WordQuestQuestionSubsystem.h"
#include "Algo/RandomShuffle.h"

void UWordQuestQuestionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    BuildYear4QuestionBank();
    ResetAdventureQuestions();
}

void UWordQuestQuestionSubsystem::ResetAdventureQuestions()
{
    RemainingQuestionIndices.Empty();
    for (int32 i = 0; i < QuestionBank.Num(); ++i) RemainingQuestionIndices.Add(i);
    Algo::RandomShuffle(RemainingQuestionIndices);
}

bool UWordQuestQuestionSubsystem::GetNextQuestion(FWordQuestQuestion& OutQuestion)
{
    if (RemainingQuestionIndices.IsEmpty()) return false;
    const int32 Index = RemainingQuestionIndices.Pop();
    OutQuestion = QuestionBank[Index];
    return true;
}

void UWordQuestQuestionSubsystem::BuildYear4QuestionBank()
{
    QuestionBank.Empty();
    auto Add = [this](const TCHAR* Id, const TCHAR* Prompt, std::initializer_list<const TCHAR*> Options, int32 Correct, const TCHAR* Category)
    {
        FWordQuestQuestion Q;
        Q.Id = FName(Id); Q.Prompt = Prompt; Q.CorrectAnswerIndex = Correct; Q.Category = Category;
        for (const TCHAR* Option : Options) Q.Answers.Add(Option);
        QuestionBank.Add(Q);
    };

    Add(TEXT("Y4_001"), TEXT("Aina ___ to school every morning."), {TEXT("walk"), TEXT("walks"), TEXT("walking"), TEXT("walked")}, 1, TEXT("Grammar"));
    Add(TEXT("Y4_002"), TEXT("Yesterday, we ___ football after school."), {TEXT("play"), TEXT("plays"), TEXT("played"), TEXT("playing")}, 2, TEXT("Tenses"));
    Add(TEXT("Y4_003"), TEXT("The cat is hiding ___ the table."), {TEXT("under"), TEXT("during"), TEXT("because"), TEXT("quickly")}, 0, TEXT("Prepositions"));
    Add(TEXT("Y4_004"), TEXT("Which word means the opposite of 'noisy'?"), {TEXT("loud"), TEXT("quiet"), TEXT("busy"), TEXT("bright")}, 1, TEXT("Vocabulary"));
    Add(TEXT("Y4_005"), TEXT("Choose the correct spelling."), {TEXT("becaus"), TEXT("becouse"), TEXT("because"), TEXT("beacause")}, 2, TEXT("Spelling"));
    Add(TEXT("Y4_006"), TEXT("Sara is ___ than her younger sister."), {TEXT("tall"), TEXT("taller"), TEXT("tallest"), TEXT("more tall")}, 1, TEXT("Comparatives"));
    Add(TEXT("Y4_007"), TEXT("I was hungry, ___ I ate a sandwich."), {TEXT("so"), TEXT("but"), TEXT("or"), TEXT("until")}, 0, TEXT("Conjunctions"));
    Add(TEXT("Y4_008"), TEXT("Which sentence is punctuated correctly?"), {TEXT("Where are you going."), TEXT("Where are you going?"), TEXT("where are you going?"), TEXT("Where are you going!")}, 1, TEXT("Punctuation"));
    Add(TEXT("Y4_009"), TEXT("The pupils ___ reading a story now."), {TEXT("is"), TEXT("are"), TEXT("was"), TEXT("be")}, 1, TEXT("Grammar"));
    Add(TEXT("Y4_010"), TEXT("Which word is an adjective?"), {TEXT("beautiful"), TEXT("quickly"), TEXT("jump"), TEXT("school")}, 0, TEXT("Word Classes"));
    Add(TEXT("Y4_011"), TEXT("My father ___ dinner last night."), {TEXT("cook"), TEXT("cooks"), TEXT("cooked"), TEXT("cooking")}, 2, TEXT("Tenses"));
    Add(TEXT("Y4_012"), TEXT("We use an umbrella when it is ___."), {TEXT("raining"), TEXT("sleeping"), TEXT("reading"), TEXT("singing")}, 0, TEXT("Vocabulary"));
    Add(TEXT("Y4_013"), TEXT("Which word is a conjunction?"), {TEXT("and"), TEXT("slowly"), TEXT("green"), TEXT("teacher")}, 0, TEXT("Word Classes"));
    Add(TEXT("Y4_014"), TEXT("There ___ three books on the desk."), {TEXT("is"), TEXT("are"), TEXT("was"), TEXT("has")}, 1, TEXT("Grammar"));
    Add(TEXT("Y4_015"), TEXT("Amir can swim, ___ he cannot dive."), {TEXT("but"), TEXT("because"), TEXT("so"), TEXT("and then")}, 0, TEXT("Conjunctions"));
    Add(TEXT("Y4_016"), TEXT("Which word means almost the same as 'happy'?"), {TEXT("glad"), TEXT("angry"), TEXT("tired"), TEXT("afraid")}, 0, TEXT("Vocabulary"));
    Add(TEXT("Y4_017"), TEXT("The rabbit ran ___."), {TEXT("quick"), TEXT("quickly"), TEXT("quicker noun"), TEXT("quickness")}, 1, TEXT("Adverbs"));
    Add(TEXT("Y4_018"), TEXT("Tomorrow, I ___ visit my grandmother."), {TEXT("will"), TEXT("did"), TEXT("was"), TEXT("has")}, 0, TEXT("Future Tense"));
    Add(TEXT("Y4_019"), TEXT("Choose the correct plural of 'child'."), {TEXT("childs"), TEXT("childes"), TEXT("children"), TEXT("childrens")}, 2, TEXT("Grammar"));
    Add(TEXT("Y4_020"), TEXT("Which sentence is a question?"), {TEXT("Please close the door."), TEXT("The door is closed."), TEXT("Did you close the door?"), TEXT("Close the door!")}, 2, TEXT("Sentence Types"));
}

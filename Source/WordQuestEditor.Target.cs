using UnrealBuildTool;
using System.Collections.Generic;

public class WordQuestEditorTarget : TargetRules
{
    public WordQuestEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("WordQuest");
    }
}

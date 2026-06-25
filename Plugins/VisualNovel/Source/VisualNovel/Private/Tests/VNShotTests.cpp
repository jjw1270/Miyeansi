// Copyright (c) 2026 장윤제. All rights reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "VNChoiceShot.h"
#include "VNDialogueShot.h"

namespace VNShotTests
{
	FVNStateChange MakeBoolChange(FName _key, EVNStateOp _op, bool _value = true)
	{
		FVNStateChange state_change;
		state_change.Domain = EVNStateDomain::Bool;
		state_change.Key = _key;
		state_change.Op = _op;
		state_change.BoolValue = _value;
		return state_change;
	}

	FVNStateChange MakeIntChange(FName _key, EVNStateOp _op, int32 _value)
	{
		FVNStateChange state_change;
		state_change.Domain = EVNStateDomain::Int;
		state_change.Key = _key;
		state_change.Op = _op;
		state_change.IntValue = _value;
		return state_change;
	}

	FVNCondition MakeBoolCondition(FName _key, EVNCompareOp _op, bool _value = true)
	{
		FVNCondition condition;
		condition.Domain = EVNStateDomain::Bool;
		condition.Key = _key;
		condition.Op = _op;
		condition.BoolValue = _value;
		return condition;
	}

	FVNCondition MakeIntCondition(FName _key, EVNCompareOp _op, int32 _value)
	{
		FVNCondition condition;
		condition.Domain = EVNStateDomain::Int;
		condition.Key = _key;
		condition.Op = _op;
		condition.IntValue = _value;
		return condition;
	}

	FVNDialogueLine MakeLine(const TCHAR* _text, const TArray<FVNStateChange>& _on_show)
	{
		FVNDialogueLine line;
		line.Text = FText::FromString(_text);
		line.OnShow = _on_show;
		return line;
	}

	FVNChoiceOption MakeOption(FName _choice_id, const TCHAR* _text)
	{
		FVNChoiceOption option;
		option.ChoiceID = _choice_id;
		option.Text = FText::FromString(_text);
		return option;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVNDialogueShotStateFlowTest,
	"VisualNovel.Shot.Dialogue.StateFlow",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVNDialogueShotStateFlowTest::RunTest(const FString& _parameters)
{
	using namespace VNShotTests;

	const FName EnteredKey(TEXT("DialogueEntered"));
	const FName LineCountKey(TEXT("LineCount"));
	const FName CompletedKey(TEXT("DialogueCompleted"));

	UVNDialogueShot* dialogue_shot = NewObject<UVNDialogueShot>();

	TArray<FVNStateChange> on_enter;
	on_enter.Add(MakeBoolChange(EnteredKey, EVNStateOp::Set, true));
	dialogue_shot->SetOnEnter(on_enter);

	TArray<FVNDialogueLine> lines;
	lines.Add(MakeLine(TEXT("첫 번째 줄"), { MakeIntChange(LineCountKey, EVNStateOp::Add, 1) }));
	lines.Add(MakeLine(TEXT("두 번째 줄"), { MakeIntChange(LineCountKey, EVNStateOp::Add, 1) }));
	dialogue_shot->SetLines(lines);

	TArray<FVNStateChange> on_complete;
	on_complete.Add(MakeBoolChange(CompletedKey, EVNStateOp::Set, true));
	dialogue_shot->SetOnComplete(on_complete);

	FVNStoryState story_state;
	TestTrue(TEXT("BeginDialogue applies OnEnter and first line OnShow"), dialogue_shot->BeginDialogueInStoryState(story_state));
	TestTrue(TEXT("OnEnter bool is set"), story_state.BoolMap.FindRef(EnteredKey));
	TestEqual(TEXT("First line OnShow is applied"), story_state.IntMap.FindRef(LineCountKey), 1);
	TestEqual(TEXT("Current line index points to first line"), dialogue_shot->GetCurrentLineIndex(), 0);

	TestTrue(TEXT("AdvanceLine applies second line OnShow"), dialogue_shot->AdvanceLineInStoryState(story_state));
	TestEqual(TEXT("Second line increments line count"), story_state.IntMap.FindRef(LineCountKey), 2);
	TestEqual(TEXT("Current line index points to second line"), dialogue_shot->GetCurrentLineIndex(), 1);

	TestTrue(TEXT("Advancing past last line completes dialogue"), dialogue_shot->AdvanceLineInStoryState(story_state));
	TestTrue(TEXT("OnComplete bool is set"), story_state.BoolMap.FindRef(CompletedKey));
	TestTrue(TEXT("Dialogue is marked complete"), dialogue_shot->HasCompletedDialogue());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVNChoiceShotVisibilityTest,
	"VisualNovel.Shot.Choice.Visibility",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVNChoiceShotVisibilityTest::RunTest(const FString& _parameters)
{
	using namespace VNShotTests;

	const FName TrustKey(TEXT("HayeonTrust"));
	const FName SecretKey(TEXT("HasSecretChoice"));

	UVNChoiceShot* choice_shot = NewObject<UVNChoiceShot>();
	TArray<FVNChoiceOption> options;

	FVNChoiceOption hayeon_option = MakeOption(TEXT("Hayeon"), TEXT("하연을 찾는다"));
	hayeon_option.ShowCond.All.Add(MakeBoolCondition(SecretKey, EVNCompareOp::Equal, true));
	hayeon_option.EnableCond.All.Add(MakeIntCondition(TrustKey, EVNCompareOp::GreaterEqual, 6));
	options.Add(hayeon_option);

	FVNChoiceOption alone_option = MakeOption(TEXT("Alone"), TEXT("혼자 확인한다"));
	options.Add(alone_option);

	FVNChoiceOption locked_option = MakeOption(TEXT("Locked"), TEXT("잠긴 선택지"));
	locked_option.EnableCond.All.Add(MakeBoolCondition(TEXT("MissingFlag"), EVNCompareOp::Exists));
	locked_option.DisabledText = FText::FromString(TEXT("조건 부족"));
	options.Add(locked_option);

	FVNChoiceOption hidden_option = MakeOption(TEXT("Hidden"), TEXT("숨김 선택지"));
	hidden_option.ShowCond.All.Add(MakeBoolCondition(TEXT("NeverShown"), EVNCompareOp::Exists));
	options.Add(hidden_option);

	choice_shot->SetOptions(options);

	FVNStoryState story_state;
	story_state.BoolMap.Add(SecretKey, true);
	story_state.IntMap.Add(TrustKey, 5);

	const TArray<FVNChoiceOptionState> visible_options = choice_shot->GetVisibleOptionsFromStoryState(story_state);
	TestEqual(TEXT("Hidden option is filtered out"), visible_options.Num(), 3);
	if (visible_options.Num() == 3)
	{
		TestEqual(TEXT("First visible option keeps source index"), visible_options[0].OptionIndex, 0);
		TestFalse(TEXT("Hayeon option is disabled when trust is too low"), visible_options[0].IsEnabled);
		TestTrue(TEXT("Alone option is enabled by empty conditions"), visible_options[1].IsEnabled);
		TestFalse(TEXT("Locked option is visible but disabled"), visible_options[2].IsEnabled);
	}

	story_state.IntMap[TrustKey] = 6;
	const TArray<FVNChoiceOptionState> updated_options = choice_shot->GetVisibleOptionsFromStoryState(story_state);
	TestTrue(TEXT("Hayeon option becomes enabled at trust threshold"), updated_options.Num() > 0 && updated_options[0].IsEnabled);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVNChoiceShotSelectionTest,
	"VisualNovel.Shot.Choice.Selection",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVNChoiceShotSelectionTest::RunTest(const FString& _parameters)
{
	using namespace VNShotTests;

	const FName ResultKey(TEXT("DDayChoice"));
	const FName EnteredKey(TEXT("ChoiceEntered"));
	const FName TrustKey(TEXT("HayeonTrust"));
	const FName CompletedKey(TEXT("ChoiceCompleted"));

	UVNChoiceShot* choice_shot = NewObject<UVNChoiceShot>();
	choice_shot->SetResultKey(ResultKey);
	choice_shot->SetOnEnter({ MakeBoolChange(EnteredKey, EVNStateOp::Set, true) });
	choice_shot->SetOnComplete({ MakeBoolChange(CompletedKey, EVNStateOp::Set, true) });

	FVNChoiceOption hayeon_option = MakeOption(TEXT("Hayeon"), TEXT("하연을 찾는다"));
	hayeon_option.OnSelect.Add(MakeIntChange(TrustKey, EVNStateOp::Add, 1));

	FVNChoiceOption disabled_option = MakeOption(TEXT("Locked"), TEXT("잠긴 선택지"));
	disabled_option.EnableCond.All.Add(MakeBoolCondition(TEXT("MissingFlag"), EVNCompareOp::Exists));

	choice_shot->SetOptions({ hayeon_option, disabled_option });

	FVNStoryState story_state;
	TestTrue(TEXT("BeginChoice applies OnEnter"), choice_shot->BeginChoiceInStoryState(story_state));
	TestTrue(TEXT("Choice OnEnter flag is set"), story_state.BoolMap.FindRef(EnteredKey));

	TestFalse(TEXT("Disabled option cannot be selected"), choice_shot->SelectOptionInStoryState(1, story_state));
	TestFalse(TEXT("Failed selection does not mark shot selected"), choice_shot->HasSelected());

	TestTrue(TEXT("Enabled option can be selected"), choice_shot->SelectOptionInStoryState(0, story_state));
	TestEqual(TEXT("ChoiceID is stored in NameMap"), story_state.NameMap.FindRef(ResultKey), FName(TEXT("Hayeon")));
	TestEqual(TEXT("OnSelect state change is applied"), story_state.IntMap.FindRef(TrustKey), 1);
	TestTrue(TEXT("OnComplete state change is applied"), story_state.BoolMap.FindRef(CompletedKey));
	TestTrue(TEXT("Successful selection marks shot selected"), choice_shot->HasSelected());
	TestEqual(TEXT("Selected ChoiceID is cached"), choice_shot->GetSelectedChoiceID(), FName(TEXT("Hayeon")));

	TestFalse(TEXT("Choice cannot be selected twice"), choice_shot->SelectOptionInStoryState(0, story_state));

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS

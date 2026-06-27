// Copyright (c) 2026 장윤제. All rights reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Engine/GameInstance.h"
#include "VNConditionBranch.h"
#include "VNStoryStateSubsystem.h"

namespace VNConditionBranchTests
{
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

	FVNStateChange MakeIntChange(FName _key, EVNStateOp _op, int32 _value)
	{
		FVNStateChange state_change;
		state_change.Domain = EVNStateDomain::Int;
		state_change.Key = _key;
		state_change.Op = _op;
		state_change.IntValue = _value;
		return state_change;
	}

	FVNConditionBranchCase MakeCase(const TCHAR* _display_name, const FVNCondition& _condition)
	{
		FVNConditionBranchCase branch_case;
		branch_case.DisplayName = FText::FromString(_display_name);
		branch_case.ConditionSet.All.Add(_condition);
		return branch_case;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVNConditionBranchOutputTest,
	"VisualNovel.ConditionBranch.Outputs",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVNConditionBranchOutputTest::RunTest(const FString& _parameters)
{
	using namespace VNConditionBranchTests;

	UVNConditionBranch* branch = NewObject<UVNConditionBranch>();
	TArray<FVNConditionBranchCase> cases;
	cases.Add(MakeCase(TEXT("Low Score"), MakeIntCondition(TEXT("TrustValue"), EVNCompareOp::Less, 3)));
	cases.Add(MakeCase(TEXT("Route Success"), MakeBoolCondition(TEXT("HasRoute"), EVNCompareOp::Equal, true)));
	branch->SetCases(cases);
	branch->SetDefaultOutputName(FText::FromString(TEXT("Fallback")));

	const TArray<FStoryBranchOutput>& outputs = branch->GetBranchOutputs();
	TestEqual(TEXT("Branch outputs are rebuilt from cases plus default"), outputs.Num(), 3);
	if (outputs.Num() == 3)
	{
		TestEqual(TEXT("First case output name is preserved"), outputs[0].DisplayName.ToString(), FString(TEXT("Low Score")));
		TestEqual(TEXT("Second case output name is preserved"), outputs[1].DisplayName.ToString(), FString(TEXT("Route Success")));
		TestEqual(TEXT("Default output is last"), outputs[2].DisplayName.ToString(), FString(TEXT("Fallback")));
	}
	TestEqual(TEXT("Default output index is case count"), branch->GetDefaultOutputIndex(), 2);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVNConditionBranchSelectionTest,
	"VisualNovel.ConditionBranch.Selection",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVNConditionBranchSelectionTest::RunTest(const FString& _parameters)
{
	using namespace VNConditionBranchTests;

	const FName TrustKey(TEXT("TrustValue"));
	const FName ResolvedKey(TEXT("IsRouteResolved"));

	UVNConditionBranch* branch = NewObject<UVNConditionBranch>();
	TArray<FVNConditionBranchCase> cases;
	cases.Add(MakeCase(TEXT("Low Score"), MakeIntCondition(TrustKey, EVNCompareOp::Less, 3)));
	cases.Add(MakeCase(TEXT("Enough Score"), MakeIntCondition(TrustKey, EVNCompareOp::GreaterEqual, 6)));
	cases.Add(MakeCase(TEXT("Route Resolved"), MakeBoolCondition(ResolvedKey, EVNCompareOp::Equal, true)));
	branch->SetCases(cases);

	FVNStoryState story_state;
	story_state.IntMap.Add(TrustKey, 6);
	story_state.BoolMap.Add(ResolvedKey, true);

	TestEqual(TEXT("First matching case wins"), branch->SelectNextIndexFromStoryState(story_state, 4), 1);

	story_state.IntMap[TrustKey] = 1;
	TestEqual(TEXT("Earlier matching case is selected"), branch->SelectNextIndexFromStoryState(story_state, 4), 0);

	story_state.IntMap[TrustKey] = 4;
	story_state.BoolMap[ResolvedKey] = false;
	TestEqual(TEXT("Default output is selected when no case passes"), branch->SelectNextIndexFromStoryState(story_state, 4), 3);

	TestEqual(TEXT("Default index is clamped to available next count"), branch->SelectNextIndexFromStoryState(story_state, 2), 1);
	TestEqual(TEXT("No output returns INDEX_NONE"), branch->SelectNextIndexFromStoryState(story_state, 0), INDEX_NONE);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVNConditionBranchNoSubsystemTest,
	"VisualNovel.ConditionBranch.NoSubsystemFallback",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVNConditionBranchNoSubsystemTest::RunTest(const FString& _parameters)
{
	using namespace VNConditionBranchTests;

	UVNConditionBranch* branch = NewObject<UVNConditionBranch>();
	TArray<FVNConditionBranchCase> cases;
	cases.Add(MakeCase(TEXT("Always Missing"), MakeBoolCondition(TEXT("Missing"), EVNCompareOp::Exists)));
	cases.Add(MakeCase(TEXT("Also Missing"), MakeBoolCondition(TEXT("AlsoMissing"), EVNCompareOp::Exists)));
	branch->SetCases(cases);

	TestEqual(TEXT("Branch falls back to default when no StoryState subsystem is available"), branch->SelectNextIndex(3), 2);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVNStoryStateSubsystemTest,
	"VisualNovel.StoryStateSubsystem.StateAccess",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVNStoryStateSubsystemTest::RunTest(const FString& _parameters)
{
	using namespace VNConditionBranchTests;

	const FName TrustKey(TEXT("TrustValue"));

	UGameInstance* game_instance = NewObject<UGameInstance>();
	UVNStoryStateSubsystem* story_state_subsystem = NewObject<UVNStoryStateSubsystem>(game_instance);
	TestNotNull(TEXT("StoryState subsystem object can be constructed for API test"), story_state_subsystem);
	if (story_state_subsystem == nullptr)
	{
		return false;
	}

	FVNStoryState story_state;
	story_state.IntMap.Add(TrustKey, 3);
	story_state_subsystem->SetStoryState(story_state);
	TestEqual(TEXT("SetStoryState stores the state"), story_state_subsystem->GetStoryStateRef().IntMap.FindRef(TrustKey), 3);

	TestTrue(TEXT("Subsystem applies a state change"), story_state_subsystem->ApplyStateChange(MakeIntChange(TrustKey, EVNStateOp::Add, 2)));
	TestEqual(TEXT("State change modifies the stored state"), story_state_subsystem->GetStoryStateRef().IntMap.FindRef(TrustKey), 5);

	story_state_subsystem->ResetStoryState();
	TestFalse(TEXT("ResetStoryState clears previous state"), story_state_subsystem->GetStoryStateRef().IntMap.Contains(TrustKey));

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS

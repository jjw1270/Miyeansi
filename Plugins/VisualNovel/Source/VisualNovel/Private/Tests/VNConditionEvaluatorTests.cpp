// Copyright (c) 2026 장윤제. All rights reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "VNConditionEvaluator.h"

namespace VNConditionEvaluatorTests
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

	FVNCondition MakeNameCondition(FName _key, EVNCompareOp _op, FName _value)
	{
		FVNCondition condition;
		condition.Domain = EVNStateDomain::Name;
		condition.Key = _key;
		condition.Op = _op;
		condition.NameValue = _value;
		return condition;
	}

	FVNCondition MakeItemCondition(EVNStateDomain _domain, const FItemID& _item_id, EVNCompareOp _op, bool _value = true)
	{
		FVNCondition condition;
		condition.Domain = _domain;
		condition.ItemID = FVNItemID(_item_id);
		condition.Op = _op;
		condition.BoolValue = _value;
		return condition;
	}

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

	FVNStateChange MakeNameChange(FName _key, EVNStateOp _op, FName _value)
	{
		FVNStateChange state_change;
		state_change.Domain = EVNStateDomain::Name;
		state_change.Key = _key;
		state_change.Op = _op;
		state_change.NameValue = _value;
		return state_change;
	}

	FVNStateChange MakeItemChange(EVNStateDomain _domain, const FItemID& _item_id, EVNStateOp _op, bool _bool_value = true, int32 _int_value = 0)
	{
		FVNStateChange state_change;
		state_change.Domain = _domain;
		state_change.ItemID = FVNItemID(_item_id);
		state_change.Op = _op;
		state_change.BoolValue = _bool_value;
		state_change.IntValue = _int_value;
		return state_change;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVNConditionEvaluatorPrimitiveTest,
	"VisualNovel.ConditionEvaluator.Primitive",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVNConditionEvaluatorPrimitiveTest::RunTest(const FString& _parameters)
{
	using namespace VNConditionEvaluatorTests;

	const FName ResolvedKey(TEXT("IsSohaResolved"));
	const FName TrustKey(TEXT("HayeonTrust"));
	const FName ChoiceKey(TEXT("DDayChoice"));
	const FName MissingKey(TEXT("MissingKey"));

	FVNStoryState story_state;
	story_state.BoolMap.Add(ResolvedKey, false);
	story_state.IntMap.Add(TrustKey, 6);
	story_state.NameMap.Add(ChoiceKey, TEXT("Hayeon"));

	TestTrue(TEXT("Bool Equal false passes when stored value is false"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeBoolCondition(ResolvedKey, EVNCompareOp::Equal, false)));
	TestFalse(TEXT("Bool Equal true fails when stored value is false"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeBoolCondition(ResolvedKey, EVNCompareOp::Equal, true)));
	TestTrue(TEXT("Bool Exists checks key presence, not stored truthiness"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeBoolCondition(ResolvedKey, EVNCompareOp::Exists)));
	TestTrue(TEXT("Bool NotExists passes for a missing key"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeBoolCondition(MissingKey, EVNCompareOp::NotExists)));

	TestTrue(TEXT("Int GreaterEqual passes at the threshold"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeIntCondition(TrustKey, EVNCompareOp::GreaterEqual, 6)));
	TestFalse(TEXT("Int Greater fails at the same value"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeIntCondition(TrustKey, EVNCompareOp::Greater, 6)));
	TestTrue(TEXT("Int Less passes below a higher threshold"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeIntCondition(TrustKey, EVNCompareOp::Less, 7)));
	TestTrue(TEXT("Int NotExists passes for a missing key"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeIntCondition(MissingKey, EVNCompareOp::NotExists, 0)));

	TestTrue(TEXT("Name Equal passes for the stored choice"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeNameCondition(ChoiceKey, EVNCompareOp::Equal, TEXT("Hayeon"))));
	TestTrue(TEXT("Name NotEqual passes for a different value"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeNameCondition(ChoiceKey, EVNCompareOp::NotEqual, TEXT("Alone"))));
	TestTrue(TEXT("Name Exists passes for a stored key"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeNameCondition(ChoiceKey, EVNCompareOp::Exists, NAME_None)));
	TestFalse(TEXT("Empty key is rejected"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeBoolCondition(NAME_None, EVNCompareOp::Exists)));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVNConditionEvaluatorPresenceTest,
	"VisualNovel.ConditionEvaluator.PresenceDomains",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVNConditionEvaluatorPresenceTest::RunTest(const FString& _parameters)
{
	using namespace VNConditionEvaluatorTests;

	const FItemID FragmentID(EItemType::Fragment, 1, 1);
	const FItemID MissingFragmentID(EItemType::Fragment, 1, 2);
	const FItemID EventID(EItemType::Event, 1, 1);
	const FItemID EndingID(EItemType::Ending, 1, 1);
	const FItemID LockedEndingID(EItemType::Ending, 1, 2);

	FVNStoryState story_state;
	story_state.Fragments.Add(FVNFragmentID(FragmentID));
	story_state.SeenEvents.Add(FVNEventID(EventID));

	FVNEndingState ending_state;
	ending_state.HasBeenSeen = true;
	ending_state.SeenCount = 1;
	story_state.EndingMap.Add(FVNEndingID(EndingID), ending_state);
	story_state.EndingMap.Add(FVNEndingID(LockedEndingID), FVNEndingState());

	TestTrue(TEXT("Fragment Exists passes when the fragment is owned"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeItemCondition(EVNStateDomain::Fragment, FragmentID, EVNCompareOp::Exists)));
	TestFalse(TEXT("Fragment Exists fails when the fragment is missing"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeItemCondition(EVNStateDomain::Fragment, MissingFragmentID, EVNCompareOp::Exists)));
	TestTrue(TEXT("Fragment Equal false passes when the fragment is missing"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeItemCondition(EVNStateDomain::Fragment, MissingFragmentID, EVNCompareOp::Equal, false)));

	TestTrue(TEXT("SeenEvent Exists passes when the event was seen"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeItemCondition(EVNStateDomain::SeenEvent, EventID, EVNCompareOp::Exists)));
	TestTrue(TEXT("Ending Exists passes only for endings marked as seen"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeItemCondition(EVNStateDomain::Ending, EndingID, EVNCompareOp::Exists)));
	TestTrue(TEXT("Ending NotExists passes for an ending entry that is still locked"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeItemCondition(EVNStateDomain::Ending, LockedEndingID, EVNCompareOp::NotExists)));
	TestFalse(TEXT("Zero ItemID is rejected"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeItemCondition(EVNStateDomain::Fragment, FItemID::Zero, EVNCompareOp::Exists)));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVNConditionEvaluatorConditionSetTest,
	"VisualNovel.ConditionEvaluator.ConditionSet",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVNConditionEvaluatorConditionSetTest::RunTest(const FString& _parameters)
{
	using namespace VNConditionEvaluatorTests;

	const FName ResolvedKey(TEXT("IsSohaResolved"));
	const FName TrustKey(TEXT("HayeonTrust"));
	const FName ChoiceKey(TEXT("DDayChoice"));
	const FName AvoidKey(TEXT("Avoid"));
	const FItemID FragmentID(EItemType::Fragment, 1, 1);

	FVNStoryState story_state;
	story_state.BoolMap.Add(ResolvedKey, true);
	story_state.IntMap.Add(TrustKey, 6);
	story_state.IntMap.Add(AvoidKey, 1);
	story_state.NameMap.Add(ChoiceKey, TEXT("Hayeon"));
	story_state.Fragments.Add(FVNFragmentID(FragmentID));

	FVNConditionSet condition_set;
	condition_set.All.Add(MakeBoolCondition(ResolvedKey, EVNCompareOp::Equal, true));
	condition_set.All.Add(MakeIntCondition(TrustKey, EVNCompareOp::GreaterEqual, 6));
	condition_set.Any.Add(MakeNameCondition(ChoiceKey, EVNCompareOp::Equal, TEXT("Alone")));
	condition_set.Any.Add(MakeItemCondition(EVNStateDomain::Fragment, FragmentID, EVNCompareOp::Exists));
	condition_set.None.Add(MakeIntCondition(AvoidKey, EVNCompareOp::GreaterEqual, 3));

	TestTrue(TEXT("ConditionSet passes when All, Any, and None rules are satisfied"),
		UVNConditionEvaluator::EvaluateConditionSet(story_state, condition_set));

	condition_set.None.Add(MakeNameCondition(ChoiceKey, EVNCompareOp::Equal, TEXT("Hayeon")));
	TestFalse(TEXT("ConditionSet fails when a None condition passes"),
		UVNConditionEvaluator::EvaluateConditionSet(story_state, condition_set));

	FVNConditionSet empty_any_set;
	empty_any_set.All.Add(MakeBoolCondition(ResolvedKey, EVNCompareOp::Equal, true));
	TestTrue(TEXT("Empty Any list is treated as pass"),
		UVNConditionEvaluator::EvaluateConditionSet(story_state, empty_any_set));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVNConditionEvaluatorStateChangeTest,
	"VisualNovel.ConditionEvaluator.StateChange",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVNConditionEvaluatorStateChangeTest::RunTest(const FString& _parameters)
{
	using namespace VNConditionEvaluatorTests;

	const FName ResolvedKey(TEXT("IsSohaResolved"));
	const FName TrustKey(TEXT("HayeonTrust"));
	const FName ChoiceKey(TEXT("DDayChoice"));
	const FItemID FragmentID(EItemType::Fragment, 1, 1);
	const FItemID EventID(EItemType::Event, 1, 1);
	const FItemID EndingID(EItemType::Ending, 1, 1);

	FVNStoryState story_state;
	story_state.CurrentDay = TEXT("DDay");

	TestTrue(TEXT("Bool Set succeeds"),
		UVNConditionEvaluator::ApplyStateChange(story_state, MakeBoolChange(ResolvedKey, EVNStateOp::Set, true)));
	TestTrue(TEXT("Bool value is stored"), story_state.BoolMap.FindRef(ResolvedKey));
	TestTrue(TEXT("Bool Remove succeeds"),
		UVNConditionEvaluator::ApplyStateChange(story_state, MakeBoolChange(ResolvedKey, EVNStateOp::Remove)));
	TestFalse(TEXT("Bool key is removed"), story_state.BoolMap.Contains(ResolvedKey));

	TestTrue(TEXT("Int Set succeeds"),
		UVNConditionEvaluator::ApplyStateChange(story_state, MakeIntChange(TrustKey, EVNStateOp::Set, 3)));
	TestTrue(TEXT("Int Add succeeds"),
		UVNConditionEvaluator::ApplyStateChange(story_state, MakeIntChange(TrustKey, EVNStateOp::Add, 4)));
	TestEqual(TEXT("Int Add accumulates"), story_state.IntMap.FindRef(TrustKey), 7);
	TestTrue(TEXT("Int Max succeeds"),
		UVNConditionEvaluator::ApplyStateChange(story_state, MakeIntChange(TrustKey, EVNStateOp::Max, 10)));
	TestEqual(TEXT("Int Max keeps the larger value"), story_state.IntMap.FindRef(TrustKey), 10);
	TestTrue(TEXT("Int Min succeeds"),
		UVNConditionEvaluator::ApplyStateChange(story_state, MakeIntChange(TrustKey, EVNStateOp::Min, 6)));
	TestEqual(TEXT("Int Min keeps the smaller value"), story_state.IntMap.FindRef(TrustKey), 6);

	TestTrue(TEXT("Name Set succeeds"),
		UVNConditionEvaluator::ApplyStateChange(story_state, MakeNameChange(ChoiceKey, EVNStateOp::Set, TEXT("Hayeon"))));
	TestEqual(TEXT("Name value is stored"), story_state.NameMap.FindRef(ChoiceKey), FName(TEXT("Hayeon")));

	TestTrue(TEXT("Fragment Set true adds the fragment"),
		UVNConditionEvaluator::ApplyStateChange(story_state, MakeItemChange(EVNStateDomain::Fragment, FragmentID, EVNStateOp::Set, true)));
	TestTrue(TEXT("Fragment is present"), story_state.Fragments.Contains(FVNFragmentID(FragmentID)));
	TestTrue(TEXT("Fragment Set false removes the fragment"),
		UVNConditionEvaluator::ApplyStateChange(story_state, MakeItemChange(EVNStateDomain::Fragment, FragmentID, EVNStateOp::Set, false)));
	TestFalse(TEXT("Fragment is absent"), story_state.Fragments.Contains(FVNFragmentID(FragmentID)));

	TestTrue(TEXT("SeenEvent Add records the event"),
		UVNConditionEvaluator::ApplyStateChange(story_state, MakeItemChange(EVNStateDomain::SeenEvent, EventID, EVNStateOp::Add)));
	TestTrue(TEXT("SeenEvent is present"), story_state.SeenEvents.Contains(FVNEventID(EventID)));

	TestTrue(TEXT("Ending Add records the ending"),
		UVNConditionEvaluator::ApplyStateChange(story_state, MakeItemChange(EVNStateDomain::Ending, EndingID, EVNStateOp::Add, true, 2)));
	const FVNEndingState* ending_state = story_state.EndingMap.Find(FVNEndingID(EndingID));
	TestNotNull(TEXT("Ending state is created"), ending_state);
	if (ending_state != nullptr)
	{
		TestTrue(TEXT("Ending is marked as seen"), ending_state->HasBeenSeen);
		TestEqual(TEXT("Ending seen count increases by IntValue"), ending_state->SeenCount, 2);
		TestEqual(TEXT("Ending last seen day is current day"), ending_state->LastSeenDay, FName(TEXT("DDay")));
	}

	TestFalse(TEXT("Empty key change is rejected"),
		UVNConditionEvaluator::ApplyStateChange(story_state, MakeIntChange(NAME_None, EVNStateOp::Set, 1)));
	TestFalse(TEXT("Zero ItemID change is rejected"),
		UVNConditionEvaluator::ApplyStateChange(story_state, MakeItemChange(EVNStateDomain::Fragment, FItemID::Zero, EVNStateOp::Add)));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVNConditionEvaluatorStateChangesBatchTest,
	"VisualNovel.ConditionEvaluator.StateChangesBatch",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVNConditionEvaluatorStateChangesBatchTest::RunTest(const FString& _parameters)
{
	using namespace VNConditionEvaluatorTests;

	const FName TrustKey(TEXT("HayeonTrust"));
	const FName ChoiceKey(TEXT("DDayChoice"));

	FVNStoryState story_state;
	TArray<FVNStateChange> state_changes;
	state_changes.Add(MakeIntChange(TrustKey, EVNStateOp::Set, 5));
	state_changes.Add(MakeIntChange(TrustKey, EVNStateOp::Add, 2));
	state_changes.Add(MakeNameChange(ChoiceKey, EVNStateOp::Set, TEXT("Hayeon")));

	TestTrue(TEXT("Valid batch reports success"), UVNConditionEvaluator::ApplyStateChanges(story_state, state_changes));
	TestEqual(TEXT("Batch applies changes in order"), story_state.IntMap.FindRef(TrustKey), 7);
	TestEqual(TEXT("Batch applies name change"), story_state.NameMap.FindRef(ChoiceKey), FName(TEXT("Hayeon")));

	state_changes.Add(MakeBoolChange(NAME_None, EVNStateOp::Set, true));
	TestFalse(TEXT("Batch reports failure when any change is invalid"), UVNConditionEvaluator::ApplyStateChanges(story_state, state_changes));

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS

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
		condition.ItemID = _item_id;
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
		state_change.ItemID = _item_id;
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

	const FName resolved_key(TEXT("IsRouteResolved"));
	const FName trust_key(TEXT("TrustValue"));
	const FName choice_key(TEXT("SelectedOption"));
	const FName missing_key(TEXT("MissingKey"));

	FVNStoryState story_state;
	story_state.BoolMap.Add(resolved_key, false);
	story_state.IntMap.Add(trust_key, 6);
	story_state.NameMap.Add(choice_key, TEXT("OptionA"));

	TestTrue(TEXT("Bool Equal false passes when stored value is false"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeBoolCondition(resolved_key, EVNCompareOp::Equal, false)));
	TestFalse(TEXT("Bool Equal true fails when stored value is false"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeBoolCondition(resolved_key, EVNCompareOp::Equal, true)));
	TestTrue(TEXT("Bool Exists checks key presence, not stored truthiness"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeBoolCondition(resolved_key, EVNCompareOp::Exists)));
	TestTrue(TEXT("Bool NotExists passes for a missing key"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeBoolCondition(missing_key, EVNCompareOp::NotExists)));

	TestTrue(TEXT("Int GreaterEqual passes at the threshold"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeIntCondition(trust_key, EVNCompareOp::GreaterEqual, 6)));
	TestFalse(TEXT("Int Greater fails at the same value"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeIntCondition(trust_key, EVNCompareOp::Greater, 6)));
	TestTrue(TEXT("Int Less passes below a higher threshold"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeIntCondition(trust_key, EVNCompareOp::Less, 7)));
	TestTrue(TEXT("Int NotExists passes for a missing key"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeIntCondition(missing_key, EVNCompareOp::NotExists, 0)));

	TestTrue(TEXT("Name Equal passes for the stored choice"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeNameCondition(choice_key, EVNCompareOp::Equal, TEXT("OptionA"))));
	TestTrue(TEXT("Name NotEqual passes for a different value"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeNameCondition(choice_key, EVNCompareOp::NotEqual, TEXT("OptionB"))));
	TestTrue(TEXT("Name Exists passes for a stored key"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeNameCondition(choice_key, EVNCompareOp::Exists, NAME_None)));
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

	const FItemID fragment_id(EItemType::Fragment, 1, 1);
	const FItemID missing_fragment_id(EItemType::Fragment, 1, 2);
	const FItemID event_id(EItemType::Event, 1, 1);
	const FItemID ending_id(EItemType::Ending, 1, 1);
	const FItemID locked_ending_id(EItemType::Ending, 1, 2);

	FVNStoryState story_state;
	story_state.Fragments.Add(FVNFragmentID(fragment_id));
	story_state.SeenEvents.Add(FVNEventID(event_id));

	FVNEndingState ending_state;
	ending_state.HasBeenSeen = true;
	ending_state.SeenCount = 1;
	story_state.EndingMap.Add(FVNEndingID(ending_id), ending_state);
	story_state.EndingMap.Add(FVNEndingID(locked_ending_id), FVNEndingState());

	TestTrue(TEXT("Fragment Exists passes when the fragment is owned"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeItemCondition(EVNStateDomain::Fragment, fragment_id, EVNCompareOp::Exists)));
	TestFalse(TEXT("Fragment Exists fails when the fragment is missing"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeItemCondition(EVNStateDomain::Fragment, missing_fragment_id, EVNCompareOp::Exists)));
	TestTrue(TEXT("Fragment Equal false passes when the fragment is missing"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeItemCondition(EVNStateDomain::Fragment, missing_fragment_id, EVNCompareOp::Equal, false)));

	TestTrue(TEXT("SeenEvent Exists passes when the event was seen"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeItemCondition(EVNStateDomain::SeenEvent, event_id, EVNCompareOp::Exists)));
	TestTrue(TEXT("Ending Exists passes only for endings marked as seen"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeItemCondition(EVNStateDomain::Ending, ending_id, EVNCompareOp::Exists)));
	TestTrue(TEXT("Ending NotExists passes for an ending entry that is still locked"),
		UVNConditionEvaluator::EvaluateCondition(story_state, MakeItemCondition(EVNStateDomain::Ending, locked_ending_id, EVNCompareOp::NotExists)));
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

	const FName resolved_key(TEXT("IsRouteResolved"));
	const FName trust_key(TEXT("TrustValue"));
	const FName choice_key(TEXT("SelectedOption"));
	const FName avoid_key(TEXT("Avoid"));
	const FItemID fragment_id(EItemType::Fragment, 1, 1);

	FVNStoryState story_state;
	story_state.BoolMap.Add(resolved_key, true);
	story_state.IntMap.Add(trust_key, 6);
	story_state.IntMap.Add(avoid_key, 1);
	story_state.NameMap.Add(choice_key, TEXT("OptionA"));
	story_state.Fragments.Add(FVNFragmentID(fragment_id));

	FVNConditionSet condition_set;
	condition_set.All.Add(MakeBoolCondition(resolved_key, EVNCompareOp::Equal, true));
	condition_set.All.Add(MakeIntCondition(trust_key, EVNCompareOp::GreaterEqual, 6));
	condition_set.Any.Add(MakeNameCondition(choice_key, EVNCompareOp::Equal, TEXT("OptionB")));
	condition_set.Any.Add(MakeItemCondition(EVNStateDomain::Fragment, fragment_id, EVNCompareOp::Exists));
	condition_set.None.Add(MakeIntCondition(avoid_key, EVNCompareOp::GreaterEqual, 3));

	TestTrue(TEXT("ConditionSet passes when All, Any, and None rules are satisfied"),
		UVNConditionEvaluator::EvaluateConditionSet(story_state, condition_set));

	condition_set.None.Add(MakeNameCondition(choice_key, EVNCompareOp::Equal, TEXT("OptionA")));
	TestFalse(TEXT("ConditionSet fails when a None condition passes"),
		UVNConditionEvaluator::EvaluateConditionSet(story_state, condition_set));

	FVNConditionSet empty_any_set;
	empty_any_set.All.Add(MakeBoolCondition(resolved_key, EVNCompareOp::Equal, true));
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

	const FName resolved_key(TEXT("IsRouteResolved"));
	const FName trust_key(TEXT("TrustValue"));
	const FName choice_key(TEXT("SelectedOption"));
	const FItemID fragment_id(EItemType::Fragment, 1, 1);
	const FItemID event_id(EItemType::Event, 1, 1);
	const FItemID ending_id(EItemType::Ending, 1, 1);

	FVNStoryState story_state;
	story_state.CurrentDay = TEXT("EventDay");

	TestTrue(TEXT("Bool Set succeeds"),
		UVNConditionEvaluator::ApplyStateChange(story_state, MakeBoolChange(resolved_key, EVNStateOp::Set, true)));
	TestTrue(TEXT("Bool value is stored"), story_state.BoolMap.FindRef(resolved_key));
	TestTrue(TEXT("Bool Remove succeeds"),
		UVNConditionEvaluator::ApplyStateChange(story_state, MakeBoolChange(resolved_key, EVNStateOp::Remove)));
	TestFalse(TEXT("Bool key is removed"), story_state.BoolMap.Contains(resolved_key));

	TestTrue(TEXT("Int Set succeeds"),
		UVNConditionEvaluator::ApplyStateChange(story_state, MakeIntChange(trust_key, EVNStateOp::Set, 3)));
	TestTrue(TEXT("Int Add succeeds"),
		UVNConditionEvaluator::ApplyStateChange(story_state, MakeIntChange(trust_key, EVNStateOp::Add, 4)));
	TestEqual(TEXT("Int Add accumulates"), story_state.IntMap.FindRef(trust_key), 7);
	TestTrue(TEXT("Int Max succeeds"),
		UVNConditionEvaluator::ApplyStateChange(story_state, MakeIntChange(trust_key, EVNStateOp::Max, 10)));
	TestEqual(TEXT("Int Max keeps the larger value"), story_state.IntMap.FindRef(trust_key), 10);
	TestTrue(TEXT("Int Min succeeds"),
		UVNConditionEvaluator::ApplyStateChange(story_state, MakeIntChange(trust_key, EVNStateOp::Min, 6)));
	TestEqual(TEXT("Int Min keeps the smaller value"), story_state.IntMap.FindRef(trust_key), 6);

	TestTrue(TEXT("Name Set succeeds"),
		UVNConditionEvaluator::ApplyStateChange(story_state, MakeNameChange(choice_key, EVNStateOp::Set, TEXT("OptionA"))));
	TestEqual(TEXT("Name value is stored"), story_state.NameMap.FindRef(choice_key), FName(TEXT("OptionA")));

	TestTrue(TEXT("Fragment Set true adds the fragment"),
		UVNConditionEvaluator::ApplyStateChange(story_state, MakeItemChange(EVNStateDomain::Fragment, fragment_id, EVNStateOp::Set, true)));
	TestTrue(TEXT("Fragment is present"), story_state.Fragments.Contains(FVNFragmentID(fragment_id)));
	TestTrue(TEXT("Fragment Set false removes the fragment"),
		UVNConditionEvaluator::ApplyStateChange(story_state, MakeItemChange(EVNStateDomain::Fragment, fragment_id, EVNStateOp::Set, false)));
	TestFalse(TEXT("Fragment is absent"), story_state.Fragments.Contains(FVNFragmentID(fragment_id)));

	TestTrue(TEXT("SeenEvent Add records the event"),
		UVNConditionEvaluator::ApplyStateChange(story_state, MakeItemChange(EVNStateDomain::SeenEvent, event_id, EVNStateOp::Add)));
	TestTrue(TEXT("SeenEvent is present"), story_state.SeenEvents.Contains(FVNEventID(event_id)));

	TestTrue(TEXT("Ending Add records the ending"),
		UVNConditionEvaluator::ApplyStateChange(story_state, MakeItemChange(EVNStateDomain::Ending, ending_id, EVNStateOp::Add, true, 2)));
	const FVNEndingState* ending_state = story_state.EndingMap.Find(FVNEndingID(ending_id));
	TestNotNull(TEXT("Ending state is created"), ending_state);
	if (ending_state != nullptr)
	{
		TestTrue(TEXT("Ending is marked as seen"), ending_state->HasBeenSeen);
		TestEqual(TEXT("Ending seen count increases by IntValue"), ending_state->SeenCount, 2);
		TestEqual(TEXT("Ending last seen day is current day"), ending_state->LastSeenDay, FName(TEXT("EventDay")));
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

	const FName trust_key(TEXT("TrustValue"));
	const FName choice_key(TEXT("SelectedOption"));

	FVNStoryState story_state;
	TArray<FVNStateChange> state_changes;
	state_changes.Add(MakeIntChange(trust_key, EVNStateOp::Set, 5));
	state_changes.Add(MakeIntChange(trust_key, EVNStateOp::Add, 2));
	state_changes.Add(MakeNameChange(choice_key, EVNStateOp::Set, TEXT("OptionA")));

	TestTrue(TEXT("Valid batch reports success"), UVNConditionEvaluator::ApplyStateChanges(story_state, state_changes));
	TestEqual(TEXT("Batch applies changes in order"), story_state.IntMap.FindRef(trust_key), 7);
	TestEqual(TEXT("Batch applies name change"), story_state.NameMap.FindRef(choice_key), FName(TEXT("OptionA")));

	state_changes.Add(MakeBoolChange(NAME_None, EVNStateOp::Set, true));
	TestFalse(TEXT("Batch reports failure when any change is invalid"), UVNConditionEvaluator::ApplyStateChanges(story_state, state_changes));

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS

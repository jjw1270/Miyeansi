// Copyright (c) 2026 장윤제. All rights reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Engine/GameInstance.h"
#include "VNEventHubSubsystem.h"

namespace VNEventHubTests
{
	FVNEventID MakeEventID(int32 _row)
	{
		return FVNEventID(FItemID(EItemType::Event, 1, _row));
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

	FVNEventDef MakeEvent(int32 _row, FName _day, EVNDaySlot _slot, int32 _priority, EVNEventRunMode _run_mode, FName _scene_id)
	{
		FVNEventDef event_def;
		event_def.EventID = MakeEventID(_row);
		event_def.DayID = _day;
		event_def.Slot = _slot;
		event_def.Priority = _priority;
		event_def.RunMode = _run_mode;
		event_def.StartSceneID = FStorySceneID(_scene_id);
		return event_def;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVNEventHubVisibleEventsTest,
	"VisualNovel.EventHub.VisibleEvents",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVNEventHubVisibleEventsTest::RunTest(const FString& _parameters)
{
	using namespace VNEventHubTests;

	const FName CurrentDay(TEXT("EventDay"));
	const FName RequiredFlag(TEXT("CanSeeAdvanced"));

	UVNEventSetAsset* event_set = NewObject<UVNEventSetAsset>();
	event_set->Events.Add(MakeEvent(1, CurrentDay, EVNDaySlot::AfterSchool, 10, EVNEventRunMode::Normal, TEXT("EventDay_Choice")));
	event_set->Events.Add(MakeEvent(2, CurrentDay, EVNDaySlot::Night, 50, EVNEventRunMode::Normal, TEXT("EventDay_Night")));
	event_set->Events.Add(MakeEvent(3, CurrentDay, EVNDaySlot::None, 30, EVNEventRunMode::Normal, TEXT("EventDay_Common")));
	event_set->Events.Add(MakeEvent(4, TEXT("DayA"), EVNDaySlot::AfterSchool, 40, EVNEventRunMode::Normal, TEXT("DayA_Event")));
	event_set->Events.Add(MakeEvent(5, CurrentDay, EVNDaySlot::AfterSchool, 90, EVNEventRunMode::Once, TEXT("EventDay_Once")));
	event_set->Events.Add(MakeEvent(6, CurrentDay, EVNDaySlot::AfterSchool, 80, EVNEventRunMode::Normal, NAME_None));

	FVNEventDef gated_event = MakeEvent(7, CurrentDay, EVNDaySlot::AfterSchool, 70, EVNEventRunMode::Normal, TEXT("EventDay_Gated"));
	gated_event.ShowCond.All.Add(MakeBoolCondition(RequiredFlag, EVNCompareOp::Equal, true));
	event_set->Events.Add(gated_event);

	UGameInstance* game_instance = NewObject<UGameInstance>();
	UVNEventHubSubsystem* event_hub = NewObject<UVNEventHubSubsystem>(game_instance);
	event_hub->AddEventSet(event_set);

	FVNStoryState story_state;
	story_state.CurrentDay = CurrentDay;
	story_state.CurrentSlot = EVNDaySlot::AfterSchool;
	story_state.SeenEvents.Add(MakeEventID(5));

	TArray<FVNEventDef> visible_events = event_hub->GetVisibleEventsFromStoryState(story_state);
	TestEqual(TEXT("Only matching day/slot, valid scene, non-seen Once, and passing ShowCond events are visible"), visible_events.Num(), 2);
	if (visible_events.Num() == 2)
	{
		TestEqual(TEXT("Visible events are sorted by priority descending"), visible_events[0].EventID, MakeEventID(3));
		TestEqual(TEXT("Second visible event is the direct slot match"), visible_events[1].EventID, MakeEventID(1));
	}

	story_state.BoolMap.Add(RequiredFlag, true);
	visible_events = event_hub->GetVisibleEventsFromStoryState(story_state);
	TestEqual(TEXT("ShowCond passing event becomes visible"), visible_events.Num(), 3);
	if (visible_events.Num() == 3)
	{
		TestEqual(TEXT("Gated event sorts by priority"), visible_events[0].EventID, MakeEventID(7));
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVNEventHubAutoSelectionTest,
	"VisualNovel.EventHub.AutoSelection",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVNEventHubAutoSelectionTest::RunTest(const FString& _parameters)
{
	using namespace VNEventHubTests;

	const FName CurrentDay(TEXT("EventDay"));
	const FName CanRunAutoKey(TEXT("CanRunAuto"));

	UVNEventSetAsset* event_set = NewObject<UVNEventSetAsset>();
	FVNEventDef low_auto = MakeEvent(1, CurrentDay, EVNDaySlot::Morning, 10, EVNEventRunMode::Auto, TEXT("Auto_Low"));
	FVNEventDef locked_auto = MakeEvent(2, CurrentDay, EVNDaySlot::Morning, 100, EVNEventRunMode::Auto, TEXT("Auto_Locked"));
	locked_auto.StartCond.All.Add(MakeBoolCondition(CanRunAutoKey, EVNCompareOp::Equal, true));
	FVNEventDef high_auto = MakeEvent(3, CurrentDay, EVNDaySlot::Morning, 50, EVNEventRunMode::Auto, TEXT("Auto_High"));
	FVNEventDef normal_event = MakeEvent(4, CurrentDay, EVNDaySlot::Morning, 200, EVNEventRunMode::Normal, TEXT("Normal_High"));

	event_set->Events = { low_auto, locked_auto, high_auto, normal_event };

	UGameInstance* game_instance = NewObject<UGameInstance>();
	UVNEventHubSubsystem* event_hub = NewObject<UVNEventHubSubsystem>(game_instance);
	event_hub->AddEventSet(event_set);

	FVNStoryState story_state;
	story_state.CurrentDay = CurrentDay;
	story_state.CurrentSlot = EVNDaySlot::Morning;

	FVNEventDef selected_event;
	TestTrue(TEXT("A startable auto event is found"), event_hub->TryFindAutoEventFromStoryState(story_state, selected_event));
	TestEqual(TEXT("Locked high priority auto is skipped until StartCond passes"), selected_event.EventID, MakeEventID(3));

	story_state.BoolMap.Add(CanRunAutoKey, true);
	TestTrue(TEXT("Auto selection still succeeds after enabling locked event"), event_hub->TryFindAutoEventFromStoryState(story_state, selected_event));
	TestEqual(TEXT("Highest priority startable auto event is selected"), selected_event.EventID, MakeEventID(2));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVNEventHubEventLifecycleTest,
	"VisualNovel.EventHub.EventLifecycle",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVNEventHubEventLifecycleTest::RunTest(const FString& _parameters)
{
	using namespace VNEventHubTests;

	const FName CurrentDay(TEXT("DayA"));
	const FName ReadyKey(TEXT("CanStartEvent"));
	const FName StartedKey(TEXT("EventStarted"));
	const FName TrustKey(TEXT("TrustValue"));
	const FName CompleteFlag(TEXT("IsEventComplete"));

	FVNEventDef event_def = MakeEvent(10, CurrentDay, EVNDaySlot::AfterSchool, 30, EVNEventRunMode::Once, TEXT("Event_Start"));
	event_def.StartCond.All.Add(MakeBoolCondition(ReadyKey, EVNCompareOp::Equal, true));
	event_def.OnStart.Add(MakeBoolChange(StartedKey, EVNStateOp::Set, true));
	event_def.OnComplete.Add(MakeIntChange(TrustKey, EVNStateOp::Add, 1));
	event_def.CompleteFlag = CompleteFlag;
	event_def.NextDay = TEXT("EventDay");
	event_def.NextSlot = EVNDaySlot::Evening;

	UGameInstance* game_instance = NewObject<UGameInstance>();
	UVNEventHubSubsystem* event_hub = NewObject<UVNEventHubSubsystem>(game_instance);

	FVNStoryState story_state;
	story_state.CurrentDay = CurrentDay;
	story_state.CurrentSlot = EVNDaySlot::AfterSchool;

	TestFalse(TEXT("BeginEvent fails while StartCond is not satisfied"), event_hub->BeginEventInStoryState(event_def, story_state));
	TestFalse(TEXT("Failed BeginEvent does not apply OnStart"), story_state.BoolMap.Contains(StartedKey));

	story_state.BoolMap.Add(ReadyKey, true);
	TestTrue(TEXT("BeginEvent applies OnStart when StartCond passes"), event_hub->BeginEventInStoryState(event_def, story_state));
	TestTrue(TEXT("OnStart bool is set"), story_state.BoolMap.FindRef(StartedKey));

	TestTrue(TEXT("CompleteEvent applies completion effects"), event_hub->CompleteEventInStoryState(event_def, story_state));
	TestTrue(TEXT("Completed event is recorded in SeenEvents"), story_state.SeenEvents.Contains(MakeEventID(10)));
	TestEqual(TEXT("OnComplete int change is applied"), story_state.IntMap.FindRef(TrustKey), 1);
	TestTrue(TEXT("CompleteFlag is set"), story_state.BoolMap.FindRef(CompleteFlag));
	TestEqual(TEXT("NextDay is applied"), story_state.CurrentDay, FName(TEXT("EventDay")));
	TestEqual(TEXT("NextSlot is applied"), story_state.CurrentSlot, EVNDaySlot::Evening);

	TestFalse(TEXT("Once event becomes hidden after completion"), event_hub->IsEventVisibleInStoryState(event_def, story_state));

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS

// Copyright (c) 2026 장윤제. All rights reserved.

#include "VNEventHubSubsystem.h"

#include "VNConditionEvaluator.h"

namespace
{
	bool IsUsableEventID(const FVNEventID& _event_id)
	{
		return static_cast<const FItemID&>(_event_id) != FItemID::Zero;
	}

	bool IsSeenEvent(const FVNStoryState& _story_state, const FVNEventID& _event_id)
	{
		return _story_state.SeenEvents.Contains(_event_id);
	}

	bool DoesDayMatch(const FVNEventDef& _event_def, const FVNStoryState& _story_state)
	{
		return !_event_def.DayID.IsNone() && _event_def.DayID == _story_state.CurrentDay;
	}

	bool DoesSlotMatch(const FVNEventDef& _event_def, const FVNStoryState& _story_state)
	{
		return _event_def.Slot == EVNDaySlot::None || _event_def.Slot == _story_state.CurrentSlot;
	}
}

void UVNEventHubSubsystem::SetEventSets(const TArray<UVNEventSetAsset*>& _event_sets)
{
	_EventSets.Reset();
	for (UVNEventSetAsset* event_set : _event_sets)
	{
		AddEventSet(event_set);
	}
}

void UVNEventHubSubsystem::AddEventSet(UVNEventSetAsset* _event_set)
{
	if (_event_set != nullptr)
	{
		_EventSets.AddUnique(_event_set);
	}
}

void UVNEventHubSubsystem::ClearEventSets()
{
	_EventSets.Reset();
}

TArray<FVNEventDef> UVNEventHubSubsystem::GetVisibleEventsFromStoryState(const FVNStoryState& _story_state) const
{
	TArray<FVNEventDef> visible_events;
	for (const UVNEventSetAsset* event_set : _EventSets)
	{
		if (event_set == nullptr)
		{
			continue;
		}

		for (const FVNEventDef& event_def : event_set->_Events)
		{
			if (IsEventVisibleInStoryState(event_def, _story_state))
			{
				visible_events.Add(event_def);
			}
		}
	}

	visible_events.Sort([](const FVNEventDef& _left, const FVNEventDef& _right)
	{
		return _left.Priority > _right.Priority;
	});

	return visible_events;
}

bool UVNEventHubSubsystem::IsEventVisibleInStoryState(const FVNEventDef& _event_def, const FVNStoryState& _story_state) const
{
	if (!IsUsableEventID(_event_def.EventID) || !_event_def.StartSceneID.IsValid())
	{
		return false;
	}

	if (!DoesDayMatch(_event_def, _story_state) || !DoesSlotMatch(_event_def, _story_state))
	{
		return false;
	}

	if (_event_def.RunMode == EVNEventRunMode::Once && IsSeenEvent(_story_state, _event_def.EventID))
	{
		return false;
	}

	return UVNConditionEvaluator::EvaluateConditionSet(_story_state, _event_def.ShowCond);
}

bool UVNEventHubSubsystem::CanStartEventInStoryState(const FVNEventDef& _event_def, const FVNStoryState& _story_state) const
{
	return IsEventVisibleInStoryState(_event_def, _story_state)
		&& UVNConditionEvaluator::EvaluateConditionSet(_story_state, _event_def.StartCond);
}

bool UVNEventHubSubsystem::TryFindAutoEventFromStoryState(const FVNStoryState& _story_state, FVNEventDef& _out_event_def) const
{
	for (const FVNEventDef& event_def : GetVisibleEventsFromStoryState(_story_state))
	{
		if (event_def.RunMode != EVNEventRunMode::Auto || !CanStartEventInStoryState(event_def, _story_state))
		{
			continue;
		}

		_out_event_def = event_def;
		return true;
	}

	return false;
}

bool UVNEventHubSubsystem::BeginEventInStoryState(const FVNEventDef& _event_def, FVNStoryState& _story_state) const
{
	if (!CanStartEventInStoryState(_event_def, _story_state))
	{
		return false;
	}

	return UVNConditionEvaluator::ApplyStateChanges(_story_state, _event_def.OnStart);
}

bool UVNEventHubSubsystem::CompleteEventInStoryState(const FVNEventDef& _event_def, FVNStoryState& _story_state) const
{
	if (!IsUsableEventID(_event_def.EventID))
	{
		return false;
	}

	_story_state.SeenEvents.Add(_event_def.EventID);

	bool is_applied = UVNConditionEvaluator::ApplyStateChanges(_story_state, _event_def.OnComplete);
	if (!_event_def.CompleteFlag.IsNone())
	{
		_story_state.BoolMap.FindOrAdd(_event_def.CompleteFlag) = true;
	}

	if (!_event_def.NextDay.IsNone())
	{
		_story_state.CurrentDay = _event_def.NextDay;
	}

	if (_event_def.NextSlot != EVNDaySlot::None)
	{
		_story_state.CurrentSlot = _event_def.NextSlot;
	}

	return is_applied;
}

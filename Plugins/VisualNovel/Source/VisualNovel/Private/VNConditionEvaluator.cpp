// Copyright (c) 2026 장윤제. All rights reserved.

#include "VNConditionEvaluator.h"

namespace
{
	bool CompareBool(bool _left, const FVNCondition& _condition)
	{
		if (_condition.Op == EVNCompareOp::Exists)
		{
			return _left;
		}

		if (_condition.Op == EVNCompareOp::NotExists)
		{
			return !_left;
		}

		if (_condition.Op == EVNCompareOp::NotEqual)
		{
			return _left != _condition.BoolValue;
		}

		return _left == _condition.BoolValue;
	}

	bool CompareInt(int32 _left, bool _exists, const FVNCondition& _condition)
	{
		switch (_condition.Op)
		{
		case EVNCompareOp::Equal:
			return _left == _condition.IntValue;
		case EVNCompareOp::NotEqual:
			return _left != _condition.IntValue;
		case EVNCompareOp::Greater:
			return _left > _condition.IntValue;
		case EVNCompareOp::GreaterEqual:
			return _left >= _condition.IntValue;
		case EVNCompareOp::Less:
			return _left < _condition.IntValue;
		case EVNCompareOp::LessEqual:
			return _left <= _condition.IntValue;
		case EVNCompareOp::Exists:
			return _exists;
		case EVNCompareOp::NotExists:
			return !_exists;
		default:
			return false;
		}
	}

	bool CompareName(FName _left, bool _exists, const FVNCondition& _condition)
	{
		switch (_condition.Op)
		{
		case EVNCompareOp::Equal:
			return _left == _condition.NameValue;
		case EVNCompareOp::NotEqual:
			return _left != _condition.NameValue;
		case EVNCompareOp::Exists:
			return _exists;
		case EVNCompareOp::NotExists:
			return !_exists;
		default:
			return false;
		}
	}

	bool ComparePresence(bool _exists, const FVNCondition& _condition)
	{
		switch (_condition.Op)
		{
		case EVNCompareOp::Equal:
			return _exists == _condition.BoolValue;
		case EVNCompareOp::NotEqual:
			return _exists != _condition.BoolValue;
		case EVNCompareOp::Exists:
			return _exists;
		case EVNCompareOp::NotExists:
			return !_exists;
		default:
			return false;
		}
	}

	bool IsUsableKey(FName _key)
	{
		return !_key.IsNone();
	}

	bool IsUsableItemID(const FItemID& _item_id)
	{
		return _item_id != FItemID::Zero;
	}
}

bool UVNConditionEvaluator::EvaluateCondition(const FVNStoryState& _story_state, const FVNCondition& _condition)
{
	switch (_condition.Domain)
	{
	case EVNStateDomain::Bool:
	{
		if (!IsUsableKey(_condition.Key))
		{
			return false;
		}

		const bool* value = _story_state.BoolMap.Find(_condition.Key);
		return CompareBool(value != nullptr ? *value : false, _condition);
	}
	case EVNStateDomain::Int:
	{
		if (!IsUsableKey(_condition.Key))
		{
			return false;
		}

		const int32* value = _story_state.IntMap.Find(_condition.Key);
		return CompareInt(value != nullptr ? *value : 0, value != nullptr, _condition);
	}
	case EVNStateDomain::Name:
	{
		if (!IsUsableKey(_condition.Key))
		{
			return false;
		}

		const FName* value = _story_state.NameMap.Find(_condition.Key);
		return CompareName(value != nullptr ? *value : NAME_None, value != nullptr, _condition);
	}
	case EVNStateDomain::Fragment:
	{
		if (!IsUsableItemID(_condition.ItemID))
		{
			return false;
		}

		return ComparePresence(_story_state.Fragments.Contains(FVNFragmentID(_condition.ItemID)), _condition);
	}
	case EVNStateDomain::SeenEvent:
	{
		if (!IsUsableItemID(_condition.ItemID))
		{
			return false;
		}

		return ComparePresence(_story_state.SeenEvents.Contains(FVNEventID(_condition.ItemID)), _condition);
	}
	case EVNStateDomain::Ending:
	{
		if (!IsUsableItemID(_condition.ItemID))
		{
			return false;
		}

		const FVNEndingState* ending_state = _story_state.EndingMap.Find(FVNEndingID(_condition.ItemID));
		return ComparePresence(ending_state != nullptr && ending_state->HasBeenSeen, _condition);
	}
	default:
		return false;
	}
}

bool UVNConditionEvaluator::EvaluateConditionSet(const FVNStoryState& _story_state, const FVNConditionSet& _condition_set)
{
	for (const FVNCondition& condition : _condition_set.All)
	{
		if (!EvaluateCondition(_story_state, condition))
		{
			return false;
		}
	}

	if (!_condition_set.Any.IsEmpty())
	{
		bool has_any_passed = false;
		for (const FVNCondition& condition : _condition_set.Any)
		{
			if (EvaluateCondition(_story_state, condition))
			{
				has_any_passed = true;
				break;
			}
		}

		if (!has_any_passed)
		{
			return false;
		}
	}

	for (const FVNCondition& condition : _condition_set.None)
	{
		if (EvaluateCondition(_story_state, condition))
		{
			return false;
		}
	}

	return true;
}

bool UVNConditionEvaluator::ApplyStateChange(FVNStoryState& _story_state, const FVNStateChange& _state_change)
{
	switch (_state_change.Domain)
	{
	case EVNStateDomain::Bool:
	{
		if (!IsUsableKey(_state_change.Key))
		{
			return false;
		}

		if (_state_change.Op == EVNStateOp::Remove)
		{
			_story_state.BoolMap.Remove(_state_change.Key);
			return true;
		}

		_story_state.BoolMap.FindOrAdd(_state_change.Key) = _state_change.BoolValue;
		return true;
	}
	case EVNStateDomain::Int:
	{
		if (!IsUsableKey(_state_change.Key))
		{
			return false;
		}

		if (_state_change.Op == EVNStateOp::Remove)
		{
			_story_state.IntMap.Remove(_state_change.Key);
			return true;
		}

		int32& value = _story_state.IntMap.FindOrAdd(_state_change.Key);
		switch (_state_change.Op)
		{
		case EVNStateOp::Add:
			value += _state_change.IntValue;
			break;
		case EVNStateOp::Max:
			value = FMath::Max(value, _state_change.IntValue);
			break;
		case EVNStateOp::Min:
			value = FMath::Min(value, _state_change.IntValue);
			break;
		case EVNStateOp::Set:
		default:
			value = _state_change.IntValue;
			break;
		}
		return true;
	}
	case EVNStateDomain::Name:
	{
		if (!IsUsableKey(_state_change.Key))
		{
			return false;
		}

		if (_state_change.Op == EVNStateOp::Remove)
		{
			_story_state.NameMap.Remove(_state_change.Key);
			return true;
		}

		_story_state.NameMap.FindOrAdd(_state_change.Key) = _state_change.NameValue;
		return true;
	}
	case EVNStateDomain::Fragment:
	{
		if (!IsUsableItemID(_state_change.ItemID))
		{
			return false;
		}

		if (_state_change.Op == EVNStateOp::Remove || (_state_change.Op == EVNStateOp::Set && !_state_change.BoolValue))
		{
			_story_state.Fragments.Remove(FVNFragmentID(_state_change.ItemID));
		}
		else
		{
			_story_state.Fragments.Add(FVNFragmentID(_state_change.ItemID));
		}
		return true;
	}
	case EVNStateDomain::SeenEvent:
	{
		if (!IsUsableItemID(_state_change.ItemID))
		{
			return false;
		}

		if (_state_change.Op == EVNStateOp::Remove || (_state_change.Op == EVNStateOp::Set && !_state_change.BoolValue))
		{
			_story_state.SeenEvents.Remove(FVNEventID(_state_change.ItemID));
		}
		else
		{
			_story_state.SeenEvents.Add(FVNEventID(_state_change.ItemID));
		}
		return true;
	}
	case EVNStateDomain::Ending:
	{
		if (!IsUsableItemID(_state_change.ItemID))
		{
			return false;
		}

		if (_state_change.Op == EVNStateOp::Remove || (_state_change.Op == EVNStateOp::Set && !_state_change.BoolValue))
		{
			_story_state.EndingMap.Remove(FVNEndingID(_state_change.ItemID));
			return true;
		}

		FVNEndingState& ending_state = _story_state.EndingMap.FindOrAdd(FVNEndingID(_state_change.ItemID));
		ending_state.HasBeenSeen = true;
		ending_state.SeenCount = _state_change.Op == EVNStateOp::Add
			? ending_state.SeenCount + FMath::Max(1, _state_change.IntValue)
			: FMath::Max(1, ending_state.SeenCount);
		ending_state.LastSeenDay = _story_state.CurrentDay;
		return true;
	}
	default:
		return false;
	}
}

bool UVNConditionEvaluator::ApplyStateChanges(FVNStoryState& _story_state, const TArray<FVNStateChange>& _state_changes)
{
	bool is_all_applied = true;
	for (const FVNStateChange& state_change : _state_changes)
	{
		is_all_applied &= ApplyStateChange(_story_state, state_change);
	}

	return is_all_applied;
}

// Copyright (c) 2026 장윤제. All rights reserved.

#include "VNStoryStateSubsystem.h"

#include "VNConditionEvaluator.h"

void UVNStoryStateSubsystem::SetStoryState(const FVNStoryState& _story_state)
{
	_StoryState = _story_state;
}

void UVNStoryStateSubsystem::ResetStoryState()
{
	_StoryState = FVNStoryState();
}

bool UVNStoryStateSubsystem::ApplyStateChange(const FVNStateChange& _state_change)
{
	return UVNConditionEvaluator::ApplyStateChange(_StoryState, _state_change);
}

bool UVNStoryStateSubsystem::ApplyStateChanges(const TArray<FVNStateChange>& _state_changes)
{
	return UVNConditionEvaluator::ApplyStateChanges(_StoryState, _state_changes);
}

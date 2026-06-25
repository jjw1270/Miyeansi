// Copyright (c) 2026 장윤제. All rights reserved.

#include "VNDialogueShot.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "VNConditionEvaluator.h"
#include "VNStoryStateSubsystem.h"

void UVNDialogueShot::OnEnterShot_Implementation()
{
	_CurrentLineIndex = INDEX_NONE;
	_HasCompletedDialogue = false;

	ApplyStateChangesToSubsystem(OnEnter);
	AdvanceLine();
}

bool UVNDialogueShot::AdvanceLine()
{
	if (_HasCompletedDialogue)
	{
		return false;
	}

	const int32 next_line_index = _CurrentLineIndex + 1;
	if (Lines.IsValidIndex(next_line_index) == false)
	{
		return CompleteDialogue();
	}

	_CurrentLineIndex = next_line_index;
	return ApplyStateChangesToSubsystem(Lines[_CurrentLineIndex].OnShow);
}

bool UVNDialogueShot::BeginDialogueInStoryState(FVNStoryState& _story_state)
{
	_CurrentLineIndex = INDEX_NONE;
	_HasCompletedDialogue = false;

	bool is_applied = UVNConditionEvaluator::ApplyStateChanges(_story_state, OnEnter);
	is_applied &= AdvanceLineInStoryState(_story_state);
	return is_applied;
}

bool UVNDialogueShot::CompleteDialogue()
{
	if (_HasCompletedDialogue)
	{
		return false;
	}

	_HasCompletedDialogue = true;
	const bool is_applied = ApplyStateChangesToSubsystem(OnComplete);
	FinishShot();
	return is_applied;
}

bool UVNDialogueShot::AdvanceLineInStoryState(FVNStoryState& _story_state)
{
	if (_HasCompletedDialogue)
	{
		return false;
	}

	const int32 next_line_index = _CurrentLineIndex + 1;
	if (Lines.IsValidIndex(next_line_index) == false)
	{
		return CompleteDialogueInStoryState(_story_state);
	}

	_CurrentLineIndex = next_line_index;
	return UVNConditionEvaluator::ApplyStateChanges(_story_state, Lines[_CurrentLineIndex].OnShow);
}

bool UVNDialogueShot::CompleteDialogueInStoryState(FVNStoryState& _story_state)
{
	if (_HasCompletedDialogue)
	{
		return false;
	}

	_HasCompletedDialogue = true;
	return UVNConditionEvaluator::ApplyStateChanges(_story_state, OnComplete);
}

bool UVNDialogueShot::ApplyStateChangesToSubsystem(const TArray<FVNStateChange>& _state_changes) const
{
	const UWorld* world = GetWorld();
	const UGameInstance* game_instance = IsValid(world) ? world->GetGameInstance() : nullptr;
	UVNStoryStateSubsystem* story_state_subsystem = IsValid(game_instance) ? game_instance->GetSubsystem<UVNStoryStateSubsystem>() : nullptr;
	if (IsValid(story_state_subsystem) == false)
	{
		return _state_changes.IsEmpty();
	}

	return story_state_subsystem->ApplyStateChanges(_state_changes);
}

// Copyright (c) 2026 장윤제. All rights reserved.

#include "VNChoiceShot.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "VNConditionEvaluator.h"
#include "VNStoryStateSubsystem.h"

void UVNChoiceShot::OnEnterShot_Implementation()
{
	_HasSelected = false;
	_SelectedChoiceID = NAME_None;

	ApplyOnEnterToSubsystem();
	SelectSingleVisibleOptionIfNeeded();
}

TArray<FVNChoiceOptionState> UVNChoiceShot::GetVisibleOptionsFromStoryState(const FVNStoryState& _story_state) const
{
	TArray<FVNChoiceOptionState> visible_options;
	for (int32 option_index = 0; option_index < _Options.Num(); ++option_index)
	{
		const FVNChoiceOption& option = _Options[option_index];
		if (IsOptionVisible(_story_state, option) == false)
		{
			continue;
		}

		FVNChoiceOptionState option_state;
		option_state.OptionIndex = option_index;
		option_state.ChoiceID = option.ChoiceID;
		option_state.Text = option.Text;
		option_state.IsEnabled = IsOptionEnabled(_story_state, option);
		option_state.DisabledText = option.DisabledText;
		visible_options.Add(option_state);
	}

	return visible_options;
}

bool UVNChoiceShot::BeginChoiceInStoryState(FVNStoryState& _story_state)
{
	_HasSelected = false;
	_SelectedChoiceID = NAME_None;

	return UVNConditionEvaluator::ApplyStateChanges(_story_state, _OnEnter);
}

bool UVNChoiceShot::SelectOption(int32 _option_index)
{
	return SelectOptionWithSubsystem(_option_index);
}

bool UVNChoiceShot::SelectOptionInStoryState(int32 _option_index, FVNStoryState& _story_state)
{
	if (_HasSelected || _Options.IsValidIndex(_option_index) == false)
	{
		return false;
	}

	const FVNChoiceOption& option = _Options[_option_index];
	if (IsOptionVisible(_story_state, option) == false || IsOptionEnabled(_story_state, option) == false)
	{
		return false;
	}

	bool is_applied = true;
	if (_ResultKey.IsNone() == false)
	{
		FVNStateChange result_change;
		result_change.Domain = EVNStateDomain::Name;
		result_change.Key = _ResultKey;
		result_change.Op = EVNStateOp::Set;
		result_change.NameValue = option.ChoiceID;
		is_applied &= UVNConditionEvaluator::ApplyStateChange(_story_state, result_change);
	}

	is_applied &= UVNConditionEvaluator::ApplyStateChanges(_story_state, option.OnSelect);
	is_applied &= UVNConditionEvaluator::ApplyStateChanges(_story_state, _OnComplete);

	_HasSelected = true;
	_SelectedChoiceID = option.ChoiceID;
	return is_applied;
}

bool UVNChoiceShot::SelectOptionByChoiceID(FName _choice_id)
{
	for (int32 option_index = 0; option_index < _Options.Num(); ++option_index)
	{
		if (_Options[option_index].ChoiceID == _choice_id)
		{
			return SelectOption(option_index);
		}
	}

	return false;
}

bool UVNChoiceShot::SelectOptionByChoiceIDInStoryState(FName _choice_id, FVNStoryState& _story_state)
{
	for (int32 option_index = 0; option_index < _Options.Num(); ++option_index)
	{
		if (_Options[option_index].ChoiceID == _choice_id)
		{
			return SelectOptionInStoryState(option_index, _story_state);
		}
	}

	return false;
}

bool UVNChoiceShot::ApplyOnEnterToSubsystem() const
{
	const UWorld* world = GetWorld();
	const UGameInstance* game_instance = IsValid(world) ? world->GetGameInstance() : nullptr;
	UVNStoryStateSubsystem* story_state_subsystem = IsValid(game_instance) ? game_instance->GetSubsystem<UVNStoryStateSubsystem>() : nullptr;
	if (IsValid(story_state_subsystem) == false)
	{
		return _OnEnter.IsEmpty();
	}

	return story_state_subsystem->ApplyStateChanges(_OnEnter);
}

bool UVNChoiceShot::SelectSingleVisibleOptionIfNeeded()
{
	if (_ShouldAutoSelectSingleOption == false)
	{
		return false;
	}

	const UWorld* world = GetWorld();
	const UGameInstance* game_instance = IsValid(world) ? world->GetGameInstance() : nullptr;
	UVNStoryStateSubsystem* story_state_subsystem = IsValid(game_instance) ? game_instance->GetSubsystem<UVNStoryStateSubsystem>() : nullptr;
	if (IsValid(story_state_subsystem) == false)
	{
		return false;
	}

	const TArray<FVNChoiceOptionState> visible_options = GetVisibleOptionsFromStoryState(story_state_subsystem->GetStoryStateRef());
	if (visible_options.Num() != 1 || visible_options[0].IsEnabled == false)
	{
		return false;
	}

	return SelectOptionWithSubsystem(visible_options[0].OptionIndex);
}

bool UVNChoiceShot::SelectOptionWithSubsystem(int32 _option_index)
{
	const UWorld* world = GetWorld();
	const UGameInstance* game_instance = IsValid(world) ? world->GetGameInstance() : nullptr;
	UVNStoryStateSubsystem* story_state_subsystem = IsValid(game_instance) ? game_instance->GetSubsystem<UVNStoryStateSubsystem>() : nullptr;
	if (IsValid(story_state_subsystem) == false)
	{
		return false;
	}

	const bool is_selected = SelectOptionInStoryState(_option_index, story_state_subsystem->GetMutableStoryState());
	if (is_selected)
	{
		FinishShot();
	}

	return is_selected;
}

bool UVNChoiceShot::IsOptionVisible(const FVNStoryState& _story_state, const FVNChoiceOption& _option) const
{
	return _option.ChoiceID.IsNone() == false && UVNConditionEvaluator::EvaluateConditionSet(_story_state, _option.ShowCond);
}

bool UVNChoiceShot::IsOptionEnabled(const FVNStoryState& _story_state, const FVNChoiceOption& _option) const
{
	return UVNConditionEvaluator::EvaluateConditionSet(_story_state, _option.EnableCond);
}

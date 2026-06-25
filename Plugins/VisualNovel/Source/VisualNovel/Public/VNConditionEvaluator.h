// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VNCondition.h"
#include "VNStateChange.h"
#include "VNStoryState.h"
#include "VNConditionEvaluator.generated.h"

UCLASS()
class VISUALNOVEL_API UVNConditionEvaluator : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "VisualNovel|Condition", meta = (ToolTip = "StoryState를 기준으로 단일 VN 조건을 평가"))
	static bool EvaluateCondition(const FVNStoryState& _story_state, const FVNCondition& _condition);

	UFUNCTION(BlueprintPure, Category = "VisualNovel|Condition", meta = (ToolTip = "All/Any/None 규칙으로 VN 조건 묶음을 평가"))
	static bool EvaluateConditionSet(const FVNStoryState& _story_state, const FVNConditionSet& _condition_set);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|State", meta = (ToolTip = "StoryState에 상태 변경 명령 하나를 적용"))
	static bool ApplyStateChange(UPARAM(ref) FVNStoryState& _story_state, const FVNStateChange& _state_change);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|State", meta = (ToolTip = "StoryState에 상태 변경 명령 배열을 순서대로 적용"))
	static bool ApplyStateChanges(UPARAM(ref) FVNStoryState& _story_state, const TArray<FVNStateChange>& _state_changes);
};
// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "VNEventSetAsset.h"
#include "VNStoryState.h"
#include "VNEventHubSubsystem.generated.h"

UCLASS(BlueprintType)
class VISUALNOVEL_API UVNEventHubSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<UVNEventSetAsset>> _EventSets;

public:
	UFUNCTION(BlueprintCallable, Category = "VisualNovel|EventHub", meta = (ToolTip = "EventHub가 조회할 이벤트 세트 목록을 교체"))
	void SetEventSets(const TArray<UVNEventSetAsset*>& _event_sets);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|EventHub", meta = (ToolTip = "EventHub가 조회할 이벤트 세트를 하나 추가"))
	void AddEventSet(UVNEventSetAsset* _event_set);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|EventHub", meta = (ToolTip = "EventHub 이벤트 세트 목록을 초기화"))
	void ClearEventSets();

	UFUNCTION(BlueprintPure, Category = "VisualNovel|EventHub", meta = (ToolTip = "현재 StoryState에서 허브 목록에 보이는 이벤트들을 반환"))
	TArray<FVNEventDef> GetVisibleEventsFromStoryState(const FVNStoryState& _story_state) const;

	UFUNCTION(BlueprintPure, Category = "VisualNovel|EventHub", meta = (ToolTip = "이벤트가 현재 StoryState에서 허브 목록에 보이는지 확인"))
	bool IsEventVisibleInStoryState(const FVNEventDef& _event_def, const FVNStoryState& _story_state) const;

	UFUNCTION(BlueprintPure, Category = "VisualNovel|EventHub", meta = (ToolTip = "이벤트를 현재 StoryState에서 실제 시작할 수 있는지 확인"))
	bool CanStartEventInStoryState(const FVNEventDef& _event_def, const FVNStoryState& _story_state) const;

	UFUNCTION(BlueprintPure, Category = "VisualNovel|EventHub", meta = (ToolTip = "시작 가능한 Auto 이벤트 중 우선순위가 가장 높은 이벤트를 찾음"))
	bool TryFindAutoEventFromStoryState(const FVNStoryState& _story_state, FVNEventDef& _out_event_def) const;

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|EventHub", meta = (ToolTip = "이벤트 시작 조건을 확인하고 OnStart 상태 변경을 적용"))
	bool BeginEventInStoryState(const FVNEventDef& _event_def, UPARAM(ref) FVNStoryState& _story_state) const;

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|EventHub", meta = (ToolTip = "이벤트 완료 처리. SeenEvents 기록, OnComplete 적용, 완료 플래그와 다음 날짜/슬롯을 갱신"))
	bool CompleteEventInStoryState(const FVNEventDef& _event_def, UPARAM(ref) FVNStoryState& _story_state) const;
};

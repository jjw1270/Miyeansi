// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StoryFlowDefines.h"
#include "VNCondition.h"
#include "VNDefines.h"
#include "VNItemIDs.h"
#include "VNStateChange.h"
#include "VNEventSetAsset.generated.h"

USTRUCT(BlueprintType, meta = (ShortToolTip = "VN 이벤트 정의"))
struct VISUALNOVEL_API FVNEventDef
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "EventHub와 SeenEvents에서 사용하는 Event 타입 ItemID"))
	FVNEventID EventID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "허브 목록, 디버그, 제작 검수 화면에 표시할 이벤트 이름"))
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "이벤트가 배치되는 날짜 ID. 예: DayA, DayB, EventDay"))
	FName DayID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "이벤트가 배치되는 하루 슬롯. None은 자동 컷신/공통 이벤트 또는 슬롯 무관 이벤트로 사용"))
	EVNDaySlot Slot = EVNDaySlot::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "자동 이벤트 충돌 해결과 허브 정렬에 쓰는 우선순위. 높을수록 먼저 선택"))
	int32 Priority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "Normal/Auto/Once/Repeatable/Summary 이벤트 재생 방식"))
	EVNEventRunMode RunMode = EVNEventRunMode::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "허브 목록에 보이기 위한 조건"))
	FVNConditionSet ShowCond;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "이벤트를 실제 시작하기 직전에 다시 확인할 조건"))
	FVNConditionSet StartCond;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "이 이벤트가 시작할 StoryFlow Scene ID"))
	FStorySceneID StartSceneID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "이벤트 시작 시 즉시 적용할 상태 변경"))
	TArray<FVNStateChange> OnStart;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "이벤트 완료 시 적용할 상태 변경"))
	TArray<FVNStateChange> OnComplete;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "이벤트 완료 시 true로 설정할 BoolMap 키. 비워두면 별도 완료 플래그를 쓰지 않음"))
	FName CompleteFlag = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "이벤트 완료 후 이동할 날짜 ID. None이면 현재 날짜를 유지"))
	FName NextDay = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "이벤트 완료 후 이동할 하루 슬롯. None이면 현재 슬롯을 유지"))
	EVNDaySlot NextSlot = EVNDaySlot::None;
};

UCLASS(BlueprintType)
class VISUALNOVEL_API UVNEventSetAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "날짜/슬롯/조건 기준으로 EventHub가 조회할 이벤트 정의 목록"))
	TArray<FVNEventDef> _Events;
};

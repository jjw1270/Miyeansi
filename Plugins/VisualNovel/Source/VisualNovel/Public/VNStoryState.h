// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "VNItemIDs.h"
#include "StoryFlowDefines.h"
#include "VNDefines.h"
#include "VNStoryState.generated.h"

USTRUCT(BlueprintType, meta = (ShortToolTip = "캐릭터별 미연시 진행 상태"))
struct VISUALNOVEL_API FVNCharState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	int32 Affinity = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	int32 Trust = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	int32 Phase = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	int32 Strain = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	EVNDaySlot LastSlot = EVNDaySlot::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FName LastDay = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	bool IsResolved = false;
};

USTRUCT(BlueprintType, meta = (ShortToolTip = "엔딩별 관람 기록"))
struct VISUALNOVEL_API FVNEndingState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	bool HasBeenSeen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	int32 SeenCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FName LastSeenDay = NAME_None;
};

USTRUCT(BlueprintType, meta = (ShortToolTip = "저장 가능한 진행 상태의 루트 구조"))
struct VISUALNOVEL_API FVNStoryState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FName CurrentDay = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FName CurrentWeek = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "현재 하루 슬롯"))
	EVNDaySlot CurrentSlot = EVNDaySlot::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "실패 루프 횟수"))
	int32 LoopCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "저장/로드 복원 위치"))
	FStoryFlowRef CurrentFlowRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "플래그 상태"))
	TMap<FName, bool> BoolMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "수치 상태"))
	TMap<FName, int32> IntMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "이름형 상태"))
	TMap<FName, FName> NameMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "캐릭터별 진행도"))
	TMap<FVNCharacterID, FVNCharState> CharMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "기억 조각/사고 단서 보유 목록"))
	TSet<FVNFragmentID> Fragments;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "엔딩별 관람/반복 기록"))
	TMap<FVNEndingID, FVNEndingState> EndingMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "이미 본 이벤트 ID"))
	TSet<FVNEventID> SeenEvents;
};
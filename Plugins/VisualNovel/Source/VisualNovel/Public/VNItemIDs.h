// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ItemID.h"
#include "VNItemIDs.generated.h"

USTRUCT(BlueprintType, meta = (ShortToolTip = "캐릭터 진행도 키로 쓰는 Character 타입 ItemID"))
struct VISUALNOVEL_API FVNCharacterID : public FItemID
{
	GENERATED_BODY()

public:
	FVNCharacterID() noexcept
		: FItemID(EItemType::Character, 0, 0)
	{
	}

	FVNCharacterID(const FItemID& _other)
		: FItemID(_other)
	{
	}
};
FORCEINLINE uint32 GetTypeHash(const FVNCharacterID& _item_id)
{
	return GetTypeHash(static_cast<const FItemID&>(_item_id));
}

USTRUCT(BlueprintType, meta = (ShortToolTip = "기억 조각/사고 단서 보유 여부에 쓰는 Fragment 타입 ItemID"))
struct VISUALNOVEL_API FVNFragmentID : public FItemID
{
	GENERATED_BODY()

public:
	FVNFragmentID() noexcept
		: FItemID(EItemType::Fragment, 0, 0)
	{
	}

	FVNFragmentID(const FItemID& _other)
		: FItemID(_other)
	{
	}
};
FORCEINLINE uint32 GetTypeHash(const FVNFragmentID& _item_id)
{
	return GetTypeHash(static_cast<const FItemID&>(_item_id));
}

USTRUCT(BlueprintType, meta = (ShortToolTip = "이미 본 이벤트 기록에 쓰는 Event 타입 ItemID"))
struct VISUALNOVEL_API FVNEventID : public FItemID
{
	GENERATED_BODY()

public:
	FVNEventID() noexcept
		: FItemID(EItemType::Event, 0, 0)
	{
	}

	FVNEventID(const FItemID& _other)
		: FItemID(_other)
	{
	}
};
FORCEINLINE uint32 GetTypeHash(const FVNEventID& _item_id)
{
	return GetTypeHash(static_cast<const FItemID&>(_item_id));
}

USTRUCT(BlueprintType, meta = (ShortToolTip = "엔딩 관람 기록에 쓰는 Ending 타입 ItemID"))
struct VISUALNOVEL_API FVNEndingID : public FItemID
{
	GENERATED_BODY()

public:
	FVNEndingID() noexcept
		: FItemID(EItemType::Ending, 0, 0)
	{
	}

	FVNEndingID(const FItemID& _other)
		: FItemID(_other)
	{
	}
};
FORCEINLINE uint32 GetTypeHash(const FVNEndingID& _item_id)
{
	return GetTypeHash(static_cast<const FItemID&>(_item_id));
}

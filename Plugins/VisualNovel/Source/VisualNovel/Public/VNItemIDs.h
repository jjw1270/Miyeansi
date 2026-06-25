// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ItemID.h"
#include "VNItemIDs.generated.h"

USTRUCT(BlueprintType)
struct VISUALNOVEL_API FVNItemID : public FItemID
{
	GENERATED_BODY()

public:
	FVNItemID() noexcept = default;

	FVNItemID(const FItemID& _other)
		: FItemID(_other)
	{
	}
};

USTRUCT(BlueprintType)
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

USTRUCT(BlueprintType)
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

USTRUCT(BlueprintType)
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

USTRUCT(BlueprintType)
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

FORCEINLINE uint32 GetTypeHash(const FVNItemID& _item_id)
{
	return GetTypeHash(static_cast<const FItemID&>(_item_id));
}

FORCEINLINE uint32 GetTypeHash(const FVNCharacterID& _item_id)
{
	return GetTypeHash(static_cast<const FItemID&>(_item_id));
}

FORCEINLINE uint32 GetTypeHash(const FVNFragmentID& _item_id)
{
	return GetTypeHash(static_cast<const FItemID&>(_item_id));
}

FORCEINLINE uint32 GetTypeHash(const FVNEventID& _item_id)
{
	return GetTypeHash(static_cast<const FItemID&>(_item_id));
}

FORCEINLINE uint32 GetTypeHash(const FVNEndingID& _item_id)
{
	return GetTypeHash(static_cast<const FItemID&>(_item_id));
}
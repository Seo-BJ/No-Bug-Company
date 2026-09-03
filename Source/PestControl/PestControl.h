// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPestControl, Log, All);

// 오브젝트 풀링 / 적 수명 검증용 전용 로그 채널.
// 기본 Verbosity=Log. 필요하면 콘솔에서 `Log LogPeCoPool Verbose` 로 상향.
DECLARE_LOG_CATEGORY_EXTERN(LogPeCoPool, Log, All);

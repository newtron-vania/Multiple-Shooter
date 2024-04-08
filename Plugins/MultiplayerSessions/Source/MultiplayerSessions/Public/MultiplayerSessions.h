// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FMultiplayerSessionsModule : public IModuleInterface
{
public:

	/** GUI 구성 요소 */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

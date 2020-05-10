#pragma once

#include "ecs/SystemBase.h"

#include <tuple>

namespace nds_se
{
	class SysScenegraph : public SystemBase<>
	{
	public:
		SysScenegraph();
		virtual void update() override;
	};
}
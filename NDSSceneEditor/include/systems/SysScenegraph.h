#pragma once

#include "ecs/SystemBase.h"

#include <tuple>

namespace nds_se
{
	//template <class... Ts>
	class SysScenegraph : public SystemBase<>
	{
	public:
		SysScenegraph();
		virtual void update() override;

	/*private:
		//typedef std::function<void(Ts)> ViewFunctionPtr;
		//ViewFunctionPtr... m_viewFunction = nullptr;

		std::tuple<std::function<void(Ts)...> m_viewFunctions;*/
	};
}
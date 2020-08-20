#pragma once

#include "CQconstant.h"

namespace CQ
{
	// �¼�����
	struct EVE
	{
		//������Ϣ���κζ���
		//���֮ǰ��������Ϣ,���ｫ���·��б�����Ϣ
		void message_ignore() noexcept;
		//���ر�����Ϣ
		void message_block() noexcept;

		int _EVEret = Msg_Ignored;

		virtual ~EVE()
		{
		}
	};


	inline void EVE::message_ignore() noexcept { _EVEret = Msg_Ignored; }
	inline void EVE::message_block() noexcept { _EVEret = Msg_Blocked; }

}

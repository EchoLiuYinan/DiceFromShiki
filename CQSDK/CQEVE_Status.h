#pragma once

#include <string>
/*
������

������ eve.data eve.dataf eve.color �����������ֲ�������

���ӳ�����ڿ�Q���̡߳��б����ã���ע��ʹ�ö������Ҫ��ʼ��(CoInitialize,CoUninitialize)��
�������ʹ���»��߿�ͷ��Ҫ�ĳ�˫�»���
*/

#define EVE_Status_EX(Name)					\
	void Name(CQ::EVEStatus & eve);\
	EVE_Status(Name)\
	{\
		CQ::EVEStatus tep;\
		Name(tep);\
		static std::string ret;\
		ret = CQ::statusEVEreturn(tep);\
		return ret.c_str();\
	}\
	void Name(CQ::EVEStatus & eve)

namespace CQ
{
	struct EVEStatus final
	{
		std::string
			//����
			data,
			//���ݵ�λ
			dataf;
		int
			// 1 : ��
			// 2 : ��
			// 3 : ��
			// 4 : ���
			// 5 : ��
			// 6 : ��
			color = 4;
		// 1 : ��
		void color_green() noexcept;
		// 2 : ��
		void color_orange() noexcept;
		// 3 : ��
		void color_red() noexcept;
		// 4 : ���
		void color_crimson() noexcept;
		// 5 : ��
		void color_black() noexcept;
		// 6 : ��
		void color_gray() noexcept;
	};

	std::string statusEVEreturn(EVEStatus& eve) noexcept;
}

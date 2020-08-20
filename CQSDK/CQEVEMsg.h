#pragma once
#include "CQEVEBasic.h"

#include <string>
#include <map>

namespace CQ
{

	//������Ϣ
	class regexMsg final
	{
		//��Ϣ
		std::map<std::string, std::string> regexMap{};
	public:
		regexMsg(const std::string& msg);
		std::string get(const std::string&);
		std::string operator [](const std::string&);
	};

	class msg;

	//��Ϣ�¼�����
	struct EVEMsg : EVE
	{
		//������
		int subType;
		//��ϢID
		int msgId;
		//��ԴQQ
		long long fromQQ;
		//��Ϣ
		std::string message;
		//����
		int font;

		EVEMsg(int subType, int msgId, long long fromQQ, std::string message, int font) noexcept;

		//��ʵ�û�
		bool isUser() const noexcept;
		//�Ƿ���ϵͳ�û�
		bool isSystem() const noexcept;

		virtual int sendMsg(const char*) const noexcept = 0;
		virtual int sendMsg(const std::string&) const noexcept = 0;
		virtual msg sendMsg() const noexcept = 0;
	};

	inline bool EVEMsg::isSystem() const noexcept { return fromQQ == 1000000; }
}

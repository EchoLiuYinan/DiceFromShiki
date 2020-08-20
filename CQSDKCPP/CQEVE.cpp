/*
���ļ�������EX�¼���ʵ��
*/
#include <exception>
#include <stdexcept>
#include "CQEVE_ALL.h"

#include "CQAPI_EX.h"
#include "CQTools.h"
#include "Unpack.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using namespace CQ;


EVEMsg::EVEMsg(const int subType, const int msgId, const long long fromQQ, std::string message, const int font) noexcept
	: subType(subType), msgId(msgId), fromQQ(fromQQ), message(move(message)), font(font)
{
}

//��ʵ�û�
bool EVEMsg::isUser() const noexcept
{
	switch (fromQQ)
	{
	case 1000000: // ϵͳ��ʾ
	case 80000000: // ����
		return false;
	default:
		return true;
	}
}

EVEGroupMsg::EVEGroupMsg(const int subType, const int msgId, const long long fromGroup, const long long fromQQ,
                         const char* fromAnonymous,
                         const char* msg, const int font) noexcept
	: EVEMsg(subType, msgId, fromQQ, msg, font), fromAnonymousInfo(), fromGroup(fromGroup),
	  fromAnonymousToken(fromAnonymous)
{
}

EVEGroupMsg::~EVEGroupMsg() noexcept { delete fromAnonymousInfo; }


bool EVEGroupMsg::isAnonymous() const noexcept { return fromQQ == 80000000; }


AnonymousInfo& EVEGroupMsg::getFromAnonymousInfo() noexcept(false)
{
	if (isAnonymous())
		return
			fromAnonymousInfo != nullptr
				? *fromAnonymousInfo
				: *(fromAnonymousInfo = new AnonymousInfo(fromAnonymousToken));
	throw std::logic_error("Trying to Get Anonymous Info from Non-anonymous User");
}

bool EVEGroupMsg::setGroupKick(const bool refusedAddAgain) const noexcept
{
	return !CQ::setGroupKick(fromGroup, fromQQ, refusedAddAgain);
}

bool EVEGroupMsg::setGroupBan(const long long banTime) const noexcept
{
	if (isAnonymous())
	{
		return !setGroupAnonymousBan(fromGroup, fromAnonymousToken, banTime);
	}
	return !CQ::setGroupBan(fromGroup, fromQQ, banTime);
}

bool EVEGroupMsg::setGroupAdmin(const bool isAdmin) const noexcept
{
	return !CQ::setGroupAdmin(fromGroup, fromQQ, isAdmin);
}

bool EVEGroupMsg::setGroupSpecialTitle(const std::string& Title, const long long ExpireTime) const noexcept
{
	return !CQ::setGroupSpecialTitle(fromGroup, fromQQ, Title, ExpireTime);
}

bool EVEGroupMsg::setGroupWholeBan(const bool isBan) const noexcept
{
	return CQ::setGroupWholeBan(fromGroup, isBan) != 0;
}

bool EVEGroupMsg::setGroupAnonymous(const bool enableAnonymous) const noexcept
{
	return CQ::setGroupAnonymous(fromGroup, enableAnonymous) != 0;
}

bool EVEGroupMsg::setGroupCard(const std::string& newGroupNick) const noexcept
{
	return CQ::setGroupCard(fromGroup, fromQQ, newGroupNick) != 0;
}

bool EVEGroupMsg::setGroupLeave(const bool isDismiss) const noexcept
{
	return CQ::setGroupLeave(fromGroup, isDismiss) != 0;
}

GroupMemberInfo EVEGroupMsg::getGroupMemberInfo(const bool disableCache) const noexcept
{
	return CQ::getGroupMemberInfo(fromGroup, fromQQ, disableCache);
}

std::vector<GroupMemberInfo> EVEGroupMsg::getGroupMemberList() const noexcept
{
	return CQ::getGroupMemberList(fromGroup);
}

EVEPrivateMsg::EVEPrivateMsg(const int subType, const int msgId, const long long fromQQ, const char* msg,
                             const int font) noexcept
	: EVEMsg(subType, msgId, fromQQ, msg, font)
{
}

//���Ժ���
bool EVEPrivateMsg::fromPrivate() const noexcept { return subType == 11; }

//��������״̬
bool EVEPrivateMsg::fromOnlineStatus() const noexcept { return subType == 1; }

//����Ⱥ��ʱ
bool EVEPrivateMsg::fromGroup() const noexcept { return subType == 2; }

//������������ʱ
bool EVEPrivateMsg::fromDiscuss() const noexcept { return subType == 3; }

msg EVEPrivateMsg::sendMsg() const noexcept { return msg(fromQQ, msgtype::Private); }
msg EVEGroupMsg::sendMsg() const noexcept { return msg(fromGroup, msgtype::Group); }
msg EVEDiscussMsg::sendMsg() const noexcept { return msg(fromQQ, msgtype::Discuss); }

int EVEPrivateMsg::sendMsg(const char* msg) const noexcept { return sendPrivateMsg(fromQQ, msg); }
int EVEPrivateMsg::sendMsg(const std::string& msg) const noexcept { return sendPrivateMsg(fromQQ, msg); }
int EVEGroupMsg::sendMsg(const char* msg) const noexcept { return sendGroupMsg(fromGroup, msg); }
int EVEGroupMsg::sendMsg(const std::string& msg) const noexcept { return sendGroupMsg(fromGroup, msg); }
int EVEDiscussMsg::sendMsg(const char* msg) const noexcept { return sendDiscussMsg(fromDiscuss, msg); }
int EVEDiscussMsg::sendMsg(const std::string& msg) const noexcept { return sendDiscussMsg(fromDiscuss, msg); }

EVEDiscussMsg::EVEDiscussMsg(const int subType, const int msgId, const long long fromDiscuss, const long long fromQQ,
                             const char* msg, const int font) noexcept
	: EVEMsg(subType, msgId, fromQQ, msg, font), fromDiscuss(fromDiscuss)
{
}

bool EVEDiscussMsg::leave() const noexcept { return !setDiscussLeave(fromDiscuss); }

void EVEStatus::color_green() noexcept { color = 1; }
void EVEStatus::color_orange() noexcept { color = 2; }
void EVEStatus::color_red() noexcept { color = 3; }
void EVEStatus::color_crimson() noexcept { color = 4; }
void EVEStatus::color_black() noexcept { color = 5; }
void EVEStatus::color_gray() noexcept { color = 6; }

std::string CQ::statusEVEreturn(EVEStatus& eve) noexcept
{
	Unpack pack;
	std::string _ret = pack.add(eve.data).add(eve.dataf).add(eve.color).getAll();
	_ret = base64_encode(_ret);
	return _ret;
}

EVERequest::EVERequest(const int sendTime, const long long fromQQ, const char* msg, const char* responseFlag) noexcept
	: sendTime(sendTime), fromQQ(fromQQ), msg(msg), responseFlag(responseFlag)
{
}

EVERequestAddFriend::EVERequestAddFriend(const int subType, const int sendTime, const long long fromQQ, const char* msg,
                                         const char* responseFlag) noexcept
	: EVERequest(sendTime, fromQQ, msg, responseFlag), subType(subType), fromGroup(0)
{
}

void EVERequestAddFriend::pass(const std::string& msg) const noexcept
{
	setFriendAddRequest(responseFlag, RequestAccepted, msg.c_str());
}

void EVERequestAddFriend::fail(const std::string& msg) const noexcept
{
	setFriendAddRequest(responseFlag, RequestRefused, msg.c_str());
}

EVERequestAddGroup::EVERequestAddGroup(const int subType, const int sendTime, const long long fromGroup,
                                       const long long fromQQ,
                                       const char* const msg, const char* const responseFlag) noexcept
	: EVERequest(sendTime, fromQQ, msg, responseFlag), subType(subType), fromGroup(fromGroup)
{
}

void EVERequestAddGroup::pass(const std::string& msg) const noexcept
{
	setGroupAddRequest(responseFlag, subType, RequestAccepted, msg.c_str());
}

void EVERequestAddGroup::fail(const std::string& msg) const noexcept
{
	setGroupAddRequest(responseFlag, subType, RequestRefused, msg.c_str());
}

AnonymousInfo::AnonymousInfo(const char* msg) noexcept
{
	if (msg != nullptr && msg[0] != '\0')
	{
		Unpack p(base64_decode(msg));
		AID = p.getLong();
		AnonymousNick = p.getstring();
	}
}

regexMsg::regexMsg(const std::string& msg)
{
	Unpack msgs(base64_decode(msg));
	auto len = msgs.getInt(); //��ȡ��������
	while (len-- > 0)
	{
		auto tep = msgs.getUnpack();
		const auto key = tep.getstring();
		const auto value = tep.getstring();
		if (key.empty())
		{
			return;
		}
		regexMap[key] = value;
	}
}

std::string regexMsg::get(const std::string& key)
{
	return regexMap[key];
}

std::string regexMsg::operator[](const std::string& key)
{
	return regexMap[key];
}
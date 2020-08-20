#include "CQAPI_EX.h"

#include <ctime>


#include "CQAPI.h"
#include "Unpack.h"
#include "CQEVE_GroupMsg.h"
#include "CQTools.h"

using namespace CQ;
using namespace std;
int lasterr;

StrangerInfo::StrangerInfo(const char* msg)
{
	if (msg != nullptr && msg[0] != '\0')
	{
		Unpack p(base64_decode(msg));
		QQID = p.getLong();
		nick = p.getstring();
		sex = p.getInt();
		age = p.getInt();
	}
}

string StrangerInfo::tostring() const
{
	return string("{")
		+ "QQ:" + to_string(QQID)
		+ " ,�ǳ�:" + nick
		+ " ,�Ա�:" + (sex == 255 ? "δ֪" : sex == 1 ? "��" : "Ů")
		+ " ,����:" + to_string(age)
		+ "}";
}

void GroupInfo::setdata(Unpack& u)
{
	llGroup = u.getLong();
	strGroupName = u.getstring();
	nGroupSize = u.getInt(); // Ⱥ����
	nMaxGroupSize = u.getInt(); //Ⱥ��ģ
	nFriendCnt = u.getInt(); //������
}

GroupInfo::GroupInfo(long long group)
{
	Unpack pack(base64_decode(CQ_getGroupInfo(getAuthCode(), group, true)));
	if (!pack.len())return;
	setdata(pack);
}

std::string GroupInfo::tostring() const
{
	return strGroupName + "(" + std::to_string(llGroup) + ")[" + std::to_string(nGroupSize) + "/" +
		std::to_string(nMaxGroupSize) + "]";
}

void GroupMemberInfo::setdata(Unpack& u)
{
	Group = u.getLong();
	QQID = u.getLong();
	Nick = u.getstring();
	GroupNick = u.getstring();
	Gender = u.getInt();
	Age = u.getInt();
	Region = u.getstring();
	AddGroupTime = u.getInt();
	LastMsgTime = u.getInt();
	LevelName = u.getstring();
	permissions = u.getInt();
	NaughtyRecord = u.getInt() == 1;
	Title = u.getstring();
	ExpireTime = u.getInt();
	canEditGroupNick = u.getInt() == 1;
}

GroupMemberInfo::GroupMemberInfo(Unpack& msg) { setdata(msg); }

GroupMemberInfo::GroupMemberInfo(const char* msg)
{
	if (msg != nullptr && msg[0] != '\0')
	{
		Unpack u(base64_decode(msg));
		setdata(u);
	}
}

GroupMemberInfo::GroupMemberInfo(const vector<unsigned char>& data)
{
	if (!data.empty())
	{
		Unpack u(data);
		setdata(u);
	}
}

string GroupMemberInfo::tostring() const
{
	string s = "{";
	s += "Ⱥ��:";
	s += to_string(Group);
	s += " ,QQ��:";
	s += to_string(QQID);
	s += " ,�ǳ�:";
	s += Nick;
	s += " ,��Ƭ:";
	s += GroupNick;
	s += " ,�Ա�:";
	s += (Gender == 255 ? "δ֪" : Gender == 1 ? "��" : "Ů");
	s += " ,����:";
	s += to_string(Age);
	s += " ,����:";
	s += Region;
	s += " ,��Ⱥʱ��:";
	s += to_string(AddGroupTime);
	s += " ,�����:";
	s += to_string(LastMsgTime);
	s += " ,�ȼ�_����:";
	s += LevelName;
	s += " ,����Ȩ��:";
	s += (permissions == 3 ? "Ⱥ��" : permissions == 2 ? "����Ա" : "ȺԱ");
	s += "(";
	s += to_string(permissions);
	s += ")";
	s += " ,������¼��Ա:";
	s += to_string(NaughtyRecord);
	s += " ,ר��ͷ��:";
	s += Title;
	s += " ,ר��ͷ�ι���ʱ��:";
	s += to_string(ExpireTime);
	s += " ,�����޸���Ƭ:";
	s += to_string(canEditGroupNick);
	s += "}";
	return s;
}

FriendInfo::FriendInfo(Unpack p)
{
	QQID = p.getLong();
	nick = p.getstring();
	remark = p.getstring();
}

std::string FriendInfo::tostring() const
{
	return remark + '(' + std::to_string(QQID) + ')' + ((remark == nick) ? "" : "��" + nick + "��");
}

//����������־
int CQ::addLog(const int Priorty, const char* Type, const char* Content) noexcept
{
	return lasterr = CQ_addLog(getAuthCode(), Priorty, Type, Content);
}

//���ͺ�����Ϣ
int CQ::sendPrivateMsg(const long long QQ, const char* msg) noexcept
{
	return CQ_sendPrivateMsg(getAuthCode(), QQ, msg);
}

//���ͺ�����Ϣ
int CQ::sendPrivateMsg(const long long QQ, const std::string& msg) noexcept { return sendPrivateMsg(QQ, msg.c_str()); }

//����Ⱥ��Ϣ
int CQ::sendGroupMsg(const long long GroupID, const char* msg) noexcept
{
	return CQ_sendGroupMsg(getAuthCode(), GroupID, msg);
}

//����Ⱥ��Ϣ
int CQ::sendGroupMsg(const long long GroupID, const std::string& msg) noexcept
{
	return sendGroupMsg(GroupID, msg.c_str());
}

int CQ::sendDiscussMsg(const long long DiscussID, const char* msg) noexcept
{
	return CQ_sendDiscussMsg(getAuthCode(), DiscussID, msg);
}

//������������Ϣ
int CQ::sendDiscussMsg(const long long DiscussID, const std::string& msg) noexcept
{
	return sendDiscussMsg(DiscussID, msg.c_str());
}

//������
int CQ::sendLike(const long long QQID, const int times) noexcept
{
	return lasterr = CQ_sendLikeV2(getAuthCode(), QQID, times);
}


//ȡCookies (���ã��˽ӿ���Ҫ�ϸ���Ȩ)
const char* CQ::getCookies(const char* Domain) noexcept { return CQ_getCookiesV2(getAuthCode(), Domain); }

//ȡCookies (���ã��˽ӿ���Ҫ�ϸ���Ȩ)
const char* CQ::getCookies(const std::string& Domain) noexcept { return getCookies(Domain.c_str()); }

//��������
const char* CQ::getRecord(const char* file, const char* outformat) noexcept
{
	return CQ_getRecordV2(getAuthCode(), file, outformat);
}

//��������
std::string CQ::getRecord(const std::string& file, const std::string& outformat) noexcept
{
	return getRecord(file.c_str(), outformat.c_str());
}

//ȡCsrfToken (���ã��˽ӿ���Ҫ�ϸ���Ȩ)
int CQ::getCsrfToken() noexcept { return CQ_getCsrfToken(getAuthCode()); }

//ȡӦ��Ŀ¼
const char* CQ::getAppDirectory() noexcept { return CQ_getAppDirectory(getAuthCode()); }

//ȡ��¼QQ
long long CQ::getLoginQQ() noexcept { return CQ_getLoginQQ(getAuthCode()); }

//ȡ��¼�ǳ�
const char* CQ::getLoginNick() noexcept { return CQ_getLoginNick(getAuthCode()); }

//��ȺԱ�Ƴ�
int CQ::setGroupKick(const long long GroupID, const long long QQID, const CQBOOL refuseForever) noexcept
{
	return lasterr = CQ_setGroupKick(getAuthCode(), GroupID, QQID, refuseForever);
}

//��ȺԱ����
int CQ::setGroupBan(const long long GroupID, const long long QQID, const long long banTime) noexcept
{
	return lasterr = CQ_setGroupBan(getAuthCode(), GroupID, QQID, banTime);
}

//��Ⱥ����Ա
int CQ::setGroupAdmin(const long long GroupID, const long long QQID, const CQBOOL isAdmin) noexcept
{
	return lasterr = CQ_setGroupAdmin(getAuthCode(), GroupID, QQID, isAdmin);
}

//��Ⱥ��Աר��ͷ��
int CQ::setGroupSpecialTitle(const long long GroupID, const long long QQID, const char* Title,
                             const long long ExpireTime) noexcept
{
	return lasterr = CQ_setGroupSpecialTitle(getAuthCode(), GroupID, QQID, Title, ExpireTime);
}

//��Ⱥ��Աר��ͷ��
int CQ::setGroupSpecialTitle(const long long GroupID, const long long QQID, const std::string& Title,
                             const long long ExpireTime) noexcept
{
	return setGroupSpecialTitle(GroupID, QQID, Title.c_str(), ExpireTime);
}

//��ȫȺ����
int CQ::setGroupWholeBan(const long long GroupID, const CQBOOL isBan) noexcept
{
	return lasterr = CQ_setGroupWholeBan(getAuthCode(), GroupID, isBan);
}

//��AnonymousȺԱ����
int CQ::setGroupAnonymousBan(const long long GroupID, const char* AnonymousToken, const long long banTime) noexcept
{
	return lasterr = CQ_setGroupAnonymousBan(getAuthCode(), GroupID, AnonymousToken, banTime);
}

//��ȺAnonymous����
int CQ::setGroupAnonymous(const long long GroupID, const CQBOOL enableAnonymous) noexcept
{
	return lasterr = CQ_setGroupAnonymous(getAuthCode(), GroupID, enableAnonymous);
}

//��Ⱥ��Ա��Ƭ
int CQ::setGroupCard(const long long GroupID, const long long QQID, const char* newGroupNick) noexcept
{
	return lasterr = CQ_setGroupCard(getAuthCode(), GroupID, QQID, newGroupNick);
}

//��Ⱥ��Ա��Ƭ
int CQ::setGroupCard(const long long GroupID, const long long QQID, const std::string& newGroupNick) noexcept
{
	return setGroupCard(GroupID, QQID, newGroupNick.c_str());
}

//��Ⱥ�˳�
int CQ::setGroupLeave(const long long GroupID, const CQBOOL isDismiss) noexcept
{
	return lasterr = CQ_setGroupLeave(getAuthCode(), GroupID, isDismiss);
}

//���������˳�
int CQ::setDiscussLeave(const long long DiscussID) noexcept
{
	return lasterr = CQ_setDiscussLeave(getAuthCode(), DiscussID);
}

//�ú�����������
int CQ::setFriendAddRequest(const char* RequestToken, const int ReturnType, const char* Remarks) noexcept
{
	return lasterr = CQ_setFriendAddRequest(getAuthCode(), RequestToken, ReturnType, Remarks);
}

//��Ⱥ��������
int CQ::setGroupAddRequest(const char* RequestToken, const int RequestType, const int ReturnType,
                           const char* Reason) noexcept
{
	return lasterr = CQ_setGroupAddRequestV2(getAuthCode(), RequestToken, RequestType, ReturnType, Reason);
}

//������������ʾ
int CQ::setFatal(const char* ErrorMsg) noexcept { return lasterr = CQ_setFatal(getAuthCode(), ErrorMsg); }

//ȡȺ��Ա��Ϣ (֧�ֻ���)
GroupMemberInfo CQ::getGroupMemberInfo(const long long GroupID, const long long QQID,
                                       const CQBOOL disableCache) noexcept
{
	return GroupMemberInfo(CQ_getGroupMemberInfoV2(getAuthCode(), GroupID, QQID, disableCache));
}

//ȡİ������Ϣ (֧�ֻ���)
StrangerInfo CQ::getStrangerInfo(const long long QQID, const CQBOOL DisableCache) noexcept
{
	return StrangerInfo(CQ_getStrangerInfo(getAuthCode(), QQID, DisableCache));
}

//ȡȺ��Ա�б�
std::vector<GroupMemberInfo> CQ::getGroupMemberList(const long long GroupID)
{
	const char* ret = CQ_getGroupMemberList(getAuthCode(), GroupID);
	if (!ret || ret[0] == '\0') return {};
	const string data(base64_decode(ret));
	if (data.empty())return {};
	vector<GroupMemberInfo> infovector;
	Unpack u(data);
	auto i = u.getInt();
	while (--i && u.len() > 0)
	{
		auto tmp = u.getUnpack();
		infovector.emplace_back(tmp);
	}

	return infovector;
}

#include <fstream>
//ȡȺ�б�
std::map<long long, std::string> CQ::getGroupList(bool disableCache)
{
	static std::map<long long, std::string> ret;
	static time_t lastUpdateTime = 0;

	const time_t timeNow = time(nullptr);
	if (!disableCache && timeNow - lastUpdateTime < 600 && !ret.empty())
	{
		return ret;
	}

	ret.clear();
	lastUpdateTime = timeNow;

	const char* src = CQ_getGroupList(getAuthCode());
	if (!src || src[0] == '\0') return {};
	const auto data(base64_decode(src)); // ����
	if (data.empty())return {};
	Unpack pack(data); // ת��ΪUnpack

	pack.getInt(); //��ȡ��Ⱥ��, ����ֵ�ڴ˲�û����
	while (pack.len() > 0)
	{
		//�������ʣ������,�ͼ�����ȡ
		auto tep = pack.getUnpack(); //��ȡ��һ��Ⱥ
		auto ID = tep.getLong(); //��ȡGroupID
		const auto name = tep.getstring(); //��ȡȺ����
		ret[ID] = name; //д��map
	}
	return ret;
}

//ȡ�����б�
std::map<long long, FriendInfo> CQ::getFriendList(bool disableCache)
{
	static std::map<long long, FriendInfo> ret;
	static time_t lastUpdateTime = 0;
	
	const time_t timeNow = time(nullptr);
	if (!disableCache && timeNow - lastUpdateTime < 600 && !ret.empty())
	{
		return ret;
	}
	
	ret.clear();
	lastUpdateTime = timeNow;
	
	const char* src = CQ_getFriendList(getAuthCode(), false);
	if (!src || src[0] == '\0') return {};
	const auto data(base64_decode(src)); // ����
	if (data.empty())return {};
	Unpack pack(data); // ��ȡԭʼ����ת��ΪUnpack
	int Cnt = pack.getInt(); //��ȡ����
	while (Cnt--)
	{
		FriendInfo info(pack.getUnpack()); //��ȡ
		ret[info.QQID] = info; //д��map
	}
	return ret;
}

bool CQ::canSendImage() noexcept
{
	return CQ_canSendImage(getAuthCode()) > 0;
}

bool CQ::canSendRecord() noexcept
{
	return CQ_canSendRecord(getAuthCode()) > 0;
}

const char* CQ::getImage(const char* file) noexcept
{
	return CQ_getImage(getAuthCode(), file);
}

const char* CQ::getImage(const std::string& file) noexcept
{
	return getImage(file.c_str());
}

int CQ::deleteMsg(const long long MsgId) noexcept
{
	return lasterr = CQ_deleteMsg(getAuthCode(), MsgId);
}

const char* CQ::getlasterrmsg() noexcept
{
	switch (lasterr)
	{
	case 0: return "�����ɹ�";
	case -1: return "������ʧ��";
	case -2: return "δ�յ��������ظ�������δ���ͳɹ�";
	case -3: return "��Ϣ������Ϊ��";
	case -4: return "��Ϣ���������쳣";
	case -5: return "��־����δ����";
	case -6: return "��־���ȼ�����";
	case -7: return "�������ʧ��";
	case -8: return "��֧�ֶ�ϵͳ�ʺŲ���";
	case -9: return "�ʺŲ��ڸ�Ⱥ�ڣ���Ϣ�޷�����";
	case -10: return "���û�������/����Ⱥ��";
	case -11: return "���ݴ����޷�������";
	case -12: return "��֧�ֶ�Anonymous��Ա�������";
	case -13: return "�޷�����Ҫ���Ե�Anonymous��Ա����";
	case -14: return "����δ֪ԭ�򣬲���ʧ��";
	case -15: return "Ⱥδ����Anonymous���Թ��ܣ���Anonymous�ʺű�����";
	case -16: return "�ʺŲ���Ⱥ�ڻ���������޷��˳�/��ɢ��Ⱥ";
	case -17: return "�ʺ�ΪȺ�����޷��˳���Ⱥ";
	case -18: return "�ʺŷ�Ⱥ�����޷���ɢ��Ⱥ";
	case -19: return "��ʱ��Ϣ��ʧЧ��δ����";
	case -20: return "��������";
	case -21: return "��ʱ��Ϣ��ʧЧ��δ����";
	case -22: return "��ȡQQ��Ϣʧ��";
	case -23: return "�Ҳ�����Ŀ��QQ�Ĺ�ϵ����Ϣ�޷�����";
	case -99: return "�����õĹ����޷��ڴ˰汾��ʵ��";
	case -101: return "Ӧ�ù���";
	case -102: return "���ǺϷ���Ӧ��";
	case -103: return "���ǺϷ���Ӧ��";
	case -104: return "Ӧ�ò����ڹ�����Information����";
	case -105: return "�޷�����Ӧ����Ϣ";
	case -106: return "�ļ�����Ӧ��ID��ͬ";
	case -107: return "������Ϣ��������";
	case -108: return "AppInfo���ص�Api�汾��֧��ֱ�Ӽ��أ���֧��Api�汾Ϊ9(������)��Ӧ��ֱ�Ӽ���";
	case -109: return "AppInfo���ص�AppID����";
	case -110: return "ȱʧAppInfo���ص�AppID��Ӧ��[Appid].json�ļ�";
	case -111: return "[Appid].json�ļ��ڵ�AppID�����ļ�����ͬ";
	case -120: return "��Api��Ȩ���պ���(Initialize)";
	case -121: return "Api��Ȩ���պ���(Initialize)����ֵ��0";
	case -122: return "���Զ����޸Ŀ�Q�����ļ�����ȡ�����ز��رտ�Q";
	case -150: return "�޷�����Ӧ����Ϣ";
	case -151: return "Ӧ����ϢJson������ʧ�ܣ�����Json���Ƿ���ȷ";
	case -152: return "Api�汾���ɻ����";
	case -153: return "Ӧ����Ϣ��������ȱʧ";
	case -154: return "Appid���Ϸ�";
	case -160: return "�¼�����(Type)�����ȱʧ";
	case -161: return "�¼�����(Function)�����ȱʧ";
	case -162: return "Ӧ�����ȼ���Ϊ10000��20000��30000��40000�е�һ��";
	case -163: return "�¼�����(Api)��֧��Ӧ��Api�汾";
	case -164: return "Ӧ��Api�汾����8����ʹ�����°汾��ͣ�õ��¼�����(Type)��1(������Ϣ)��3(��ʱ��Ϣ)";
	case -165: return "�¼�����Ϊ2(Ⱥ��Ϣ)��4(��������Ϣ)��21(˽����Ϣ)����ȱ���������ʽ(regex)�ı���ʽ����(expression)";
	case -166: return "����Ϊ�յ��������ʽ(regex)��key";
	case -167: return "����Ϊ�յ��������ʽ(regex)�ı���ʽ����(expression)";
	case -168: return "Ӧ���¼�(event)id���������ڻ�Ϊ0";
	case -169: return "Ӧ���¼�(event)id�������ظ�";
	case -180: return "Ӧ��״̬(status)id���������ڻ�Ϊ0";
	case -181: return "Ӧ��״̬(status)period���������ڻ����ô���";
	case -182: return "Ӧ��״̬(status)id�������ظ�";
	case -201: return "�޷�����Ӧ�ã�������Ӧ���ļ�����";
	case -202: return "Api�汾���ɻ����";
	case -997: return "Ӧ��δ����";
	case -998: return "Ӧ�õ�����Auth����֮��� ��Q Api��";
	default: return "δ֪����";
	}
}
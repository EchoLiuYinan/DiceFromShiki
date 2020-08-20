#pragma once
#include "CQEVEMsg.h"

#include <vector>
/*
Ⱥ��Ϣ(Type=2)

subType �����ͣ�Ŀǰ�̶�Ϊ1
msgId ��ϢID
fromGroup ��ԴȺ��
fromQQ ��ԴQQ��
fromAnonymous ��Դ������
msg ��Ϣ����
font ����

�����Ϣ����������,isAnonymous() ���� true, ��ʹ�� getFromAnonymousInfo() ��ȡ ��������Ϣ

���ӳ�����ڿ�Q���̡߳��б����ã���ע��ʹ�ö������Ҫ��ʼ��(CoInitialize,CoUninitialize)
�������ʹ���»��߿�ͷ��Ҫ�ĳ�˫�»���
���ط���ֵ,��Ϣ��������,������Ȳ�������
*/
#define EVE_GroupMsg_EX(Name) 																	\
	void Name(CQ::EVEGroupMsg & eve);															\
	EVE_GroupMsg(Name)																		\
	{																							\
		CQ::EVEGroupMsg tep(subType, msgId, fromGroup, fromQQ, fromAnonymous, msg, font);	\
		Name(tep); \
		return tep._EVEret;																		\
	}																							\
	void Name(CQ::EVEGroupMsg & eve)

namespace CQ
{
	class GroupMemberInfo;

	// Ⱥ������Ϣ
	struct AnonymousInfo final
	{
		long long AID = 0;
		std::string AnonymousNick = "";

		explicit AnonymousInfo(const char* msg) noexcept;
		AnonymousInfo() noexcept = default;
	};

	//Ⱥ�¼�
	struct EVEGroupMsg final : public EVEMsg
	{
	private:
		AnonymousInfo* fromAnonymousInfo;
	public:
		//Ⱥ��
		long long fromGroup;
		//�����õ�����
		const char* fromAnonymousToken;
		EVEGroupMsg(int subType, int msgId, long long fromGroup, long long fromQQ, const char* fromAnonymous,
		            const char* msg, int font) noexcept;

		virtual ~EVEGroupMsg() noexcept;

		bool isAnonymous() const noexcept;

		// ͨ�� EVEMsg �̳�
		int sendMsg(const char*) const noexcept override;
		int sendMsg(const std::string&) const noexcept override;
		msg sendMsg() const noexcept override;

		//��ȡ��������Ϣ
		AnonymousInfo& getFromAnonymousInfo() noexcept(false);

		//��ȺԱ�Ƴ�
		bool setGroupKick(bool refusedAddAgain = false) const noexcept;
		//��ȺԱ����
		//�Զ��ж��Ƿ�������
		bool setGroupBan(long long banTime = 60) const noexcept;
		//��Ⱥ����Ա
		bool setGroupAdmin(bool isAdmin) const noexcept;
		//��Ⱥ��Աר��ͷ��
		bool setGroupSpecialTitle(const std::string& Title, long long ExpireTime = -1) const noexcept;

		//��ȫȺ����
		bool setGroupWholeBan(bool enableBan = true) const noexcept;
		//��Ⱥ��������
		bool setGroupAnonymous(bool enableAnonymous) const noexcept;
		//��Ⱥ��Ա��Ƭ
		bool setGroupCard(const std::string& newGroupNick) const noexcept;
		//��Ⱥ�˳�
		bool setGroupLeave(bool isDismiss) const noexcept;
		//ȡȺ��Ա��Ϣ (֧�ֻ���)
		GroupMemberInfo getGroupMemberInfo(bool disableCache = false) const noexcept;
		//ȡȺ��Ա�б�
		std::vector<GroupMemberInfo> getGroupMemberList() const noexcept;
	};
}

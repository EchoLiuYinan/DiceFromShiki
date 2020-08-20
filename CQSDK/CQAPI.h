/* 
CoolQ SDK for VS2017
Api Version 9.13
Written by MukiPy2001 & Thanks for the help of orzFly and Coxxs
*/
#pragma once
#include "cqdefine.h"
#ifdef _MSC_VER
#define CQAPI(NAME,ReturnType) extern "C" __declspec(dllimport) ReturnType __stdcall NAME  // NOLINT
#else
#define CQAPI(NAME,ReturnType) extern "C" __attribute__((dllimport)) ReturnType __attribute__((__stdcall__)) NAME  // NOLINT
#endif /*_MSC_VER*/	

namespace CQ
{
	// ��ȡ����api�����AuthCode
	int getAuthCode() noexcept;

	//���ͺ�����Ϣ 
	//Auth=106 ʧ�ܷ��ظ�ֵ,�ɹ�������ϢID 
	CQAPI(CQ_sendPrivateMsg, int)(
		int AuthCode, // 
		long long QQID, // Ŀ��QQ 
		const char* msg // ��Ϣ���� 
	) noexcept;

	//����Ⱥ��Ϣ 
	//Auth=101 ʧ�ܷ��ظ�ֵ,�ɹ�������ϢID
	CQAPI(CQ_sendGroupMsg, int)(
		int AuthCode, // 
		long long GroupID, // Ŀ��Ⱥ 
		const char* msg // ��Ϣ���� 
	) noexcept;

	//������������Ϣ 
	//Auth=103 ʧ�ܷ��ظ�ֵ,�ɹ�������ϢID
	CQAPI(CQ_sendDiscussMsg, int)(
		int AuthCode, // 
		long long DiscussID, // Ŀ�������� 
		const char* msg // ��Ϣ���� 
	) noexcept;

	//������ Auth=110
	CQAPI(CQ_sendLike, int)(
		int AuthCode, // 
		long long QQID // Ŀ��QQ 
	) noexcept;

	//������V2 Auth=110
	CQAPI(CQ_sendLikeV2, int)(
		int AuthCode, // 
		long long QQID, // Ŀ��QQ 
		int times // �޵Ĵ��������10�� 
	) noexcept;

	//ȡCookies (���ã��˽ӿ���Ҫ�ϸ���Ȩ) 
	//Auth=20 ����,�˽ӿ���Ҫ�ϸ���Ȩ 
	CQAPI(CQ_getCookiesV2, const char *)(
		int AuthCode, // 
		const char* Domain
	) noexcept;

	//�������� 
	CQAPI(CQ_getRecordV2, const char *)(
		int AuthCode, // 
		const char* file, // �յ���Ϣ�е������ļ��� (file) 
		const char* outformat // Ӧ������ĸ�ʽ  mp3,amr,wma,m4a,spx,ogg,wav,flac
	) noexcept;

	//ȡCsrfToken (���ã��˽ӿ���Ҫ�ϸ���Ȩ) 
	//Auth=20 ��QQ��ҳ�õ���bkn/g_tk�� ����,�˽ӿ���Ҫ�ϸ���Ȩ 
	CQAPI(CQ_getCsrfToken, int)(
		int AuthCode // 
	) noexcept;

	//ȡӦ��Ŀ¼ 
	//���ص�·��ĩβ��"\" 
	CQAPI(CQ_getAppDirectory, const char *)(
		int AuthCode // 
	) noexcept;
		
	//ȡ��¼QQ 
	CQAPI(CQ_getLoginQQ, long long)(
		int AuthCode // 
	) noexcept;

	//ȡ��¼�ǳ� 
	CQAPI(CQ_getLoginNick, const char *)(
		int AuthCode // 
	) noexcept;

	//��ȺԱ�Ƴ� Auth=120 
	CQAPI(CQ_setGroupKick, int)(
		int AuthCode, // 
		long long GroupID, // Ŀ��Ⱥ 
		long long QQID, // Ŀ��QQ 
		CQBOOL RefuseForever // ���Ϊ�棬�򡰲��ٽ��մ��˼�Ⱥ���롱�������� 
	) noexcept;

	//��ȺԱ���� Auth=121 
	CQAPI(CQ_setGroupBan, int)(
		int AuthCode, // 
		long long GroupID, // Ŀ��Ⱥ 
		long long QQID, // Ŀ��QQ 
		long long Time // ���Ե�ʱ�䣬��λΪ�롣���Ҫ�����������д0 
	) noexcept;

	//��Ⱥ����Ա Auth=122 
	CQAPI(CQ_setGroupAdmin, int)(
		int AuthCode, // 
		long long GroupID, // Ŀ��Ⱥ 
		long long QQID, // �����õ�QQ 
		CQBOOL setAdmin // ��/���ù���Ա ��/ȡ������Ա 
	) noexcept;

	//��Ⱥ��Աר��ͷ�� Auth=128 ��Ⱥ��Ȩ�� 
	CQAPI(CQ_setGroupSpecialTitle, int)(
		int AuthCode, // 
		long long GroupID, // Ŀ��Ⱥ 
		long long QQID, // Ŀ��QQ 
		const char* Title, // ���Ҫɾ����������� 
		long long ExpireTime // ר��ͷ����Ч�ڣ���λΪ�롣���������Ч��������д-1
	) noexcept;

	//��ȫȺ���� Auth=123 
	CQAPI(CQ_setGroupWholeBan, int)(
		int AuthCode, // 
		long long GroupID, // Ŀ��Ⱥ 
		CQBOOL EnableWholeBan // ��/���� ��/�ر� 
	) noexcept;

	//������ȺԱ���� Auth=124 
	CQAPI(CQ_setGroupAnonymousBan, int)(
		int AuthCode, // 
		long long GroupID, // Ŀ��Ⱥ 
		const char* AnonymousID, // Ⱥ��Ϣ�¼��յ��ġ����������� 
		long long time // ���Ե�ʱ�䣬��λΪ�롣��֧�ֽ�� 
	) noexcept;

	//��Ⱥ�������� Auth=125 
	CQAPI(CQ_setGroupAnonymous, int)(
		int AuthCode, // 
		long long GroupID, // 
		CQBOOL EnableAnonymous // 
	) noexcept;

	//��Ⱥ��Ա��Ƭ Auth=126 
	CQAPI(CQ_setGroupCard, int)(
		int AuthCode, // 
		long long GroupID, // Ŀ��Ⱥ 
		long long QQID, // �����õ�QQ 
		const char* NewGroupCarkNick // 
	) noexcept;

	//��Ⱥ�˳� Auth=127 ����,�˽ӿ���Ҫ�ϸ���Ȩ 
	CQAPI(CQ_setGroupLeave, int)(
		int AuthCode, // 
		long long GroupID, // Ŀ��Ⱥ 
		CQBOOL isDismiss // ��/��ɢ��Ⱥ (Ⱥ��) ��/�˳���Ⱥ (����Ⱥ��Ա) 
	) noexcept;

	//���������˳� Auth=140 
	CQAPI(CQ_setDiscussLeave, int)(
		int AuthCode, // 
		long long DiscussID // Ŀ�������� 
	) noexcept;

	//�ú���������� Auth=150 
	CQAPI(CQ_setFriendAddRequest, int)(
		int AuthCode, // 
		const char* ResponseToken, // �����¼��յ��ġ�������ʶ������ 
		int ResponseType, // #����_ͨ�� �� #����_�ܾ� 
		const char* Remarks // ��Ӻ�ĺ��ѱ�ע 
	) noexcept;

	//��Ⱥ������� Auth=151 
	CQAPI(CQ_setGroupAddRequest, int)(
		int AuthCode, // 
		const char* ResponseToken, // �����¼��յ��ġ�������ʶ������ 
		int RequestType, // ���������¼������������� #����_Ⱥ��� �� #����_Ⱥ���� 
		int ResponseType // #����_ͨ�� �� #����_�ܾ� 
	) noexcept;

	//��Ⱥ������� Auth=151 
	CQAPI(CQ_setGroupAddRequestV2, int)(
		int AuthCode, // 
		const char* RequestToken, // �����¼��յ��ġ�������ʶ������ 
		int RequestType, // ���������¼������������� #����_Ⱥ��� �� #����_Ⱥ���� 
		int ResponseType, // #����_ͨ�� �� #����_�ܾ� 
		const char* Reason // �������ɣ��� #����_Ⱥ��� �� #����_�ܾ� ʱ����
	) noexcept;

	//����������־ 
	CQAPI(CQ_addLog, int)(
		int AuthCode, // 
		int Priorty, // #Log_ ��ͷ���� 
		const char* Type, // 
		const char* Content // 
	) noexcept;

	//������������ʾ 
	CQAPI(CQ_setFatal, int)(
		int AuthCode, // 
		const char* ErrorMsg // 
	) noexcept;

	//ȡȺ��Ա��Ϣ (�ɰ�,����CQ_getGroupMemberInfoV2) Auth=130 
	CQAPI(CQ_getGroupMemberInfo, const char *)(
		int AuthCode, // 
		long long GroupID, // Ŀ��QQ����Ⱥ 
		long long QQID // Ŀ��QQ 
	) noexcept;
	//ȡȺ��Ա��Ϣ (֧�ֻ���) Auth=130 
	CQAPI(CQ_getGroupMemberInfoV2, const char *)(
		int AuthCode, // 
		long long GroupID, // Ŀ��QQ����Ⱥ 
		long long QQID, // Ŀ��QQ 
		CQBOOL DisableCache
	) noexcept;
	//ȡİ������Ϣ (֧�ֻ���) Auth=131 
	CQAPI(CQ_getStrangerInfo, const char *)(
		int AuthCode, // 
		long long QQID, // Ŀ��QQ 
		CQBOOL DisableCache
	) noexcept;
	//ȡȺ��Ϣ (֧�ֻ���) Auth=132
	CQAPI(CQ_getGroupInfo, const char*)(
		int AuthCode, // 
		long long GroupID, // Ŀ��Ⱥ
		CQBOOL DisableCache
	) noexcept;

	//ȡȺ��Ա�б� Auth=160  
	CQAPI(CQ_getGroupMemberList, const char *)(
		int AuthCode, // 
		long long GroupID // Ŀ��QQ����Ⱥ
	) noexcept;
	//ȡȺ�б� Auth=161  
	CQAPI(CQ_getGroupList, const char *)(
		int AuthCode
	) noexcept;
	//ȡ�����б� Auth=162 
	CQAPI(CQ_getFriendList, const char*)(
		int AuthCode,
		CQBOOL Reserved = false // ����Ϊfalse
		) noexcept;
	//������Ϣ Auth=180
	CQAPI(CQ_deleteMsg, int)(
		int AuthCode,
		long long MsgId
	) noexcept;

	//�Ƿ�֧�ַ���ͼƬ�����ش��� 0 Ϊ֧�֣����� 0 Ϊ��֧��
	CQAPI(CQ_canSendImage, int)(
		int AuthCode
	) noexcept;
	//�Ƿ�֧�ַ������������ش��� 0 Ϊ֧�֣����� 0 Ϊ��֧��
	CQAPI(CQ_canSendRecord, int)(
		int AuthCode
	) noexcept;
	//����ͼƬ��������ͼƬ�ļ�����·��
	CQAPI(CQ_getImage, const char*)(
		int AuthCode,
		const char* file//�յ���Ϣ�е�ͼƬ�ļ���(file)
	) noexcept;
}

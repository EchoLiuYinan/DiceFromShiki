/*
 *  _______     ________    ________    ________    __
 * |   __  \   |__    __|  |   _____|  |   _____|  |  |
 * |  |  |  |     |  |     |  |        |  |_____   |  |
 * |  |  |  |     |  |     |  |        |   _____|  |__|
 * |  |__|  |   __|  |__   |  |_____   |  |_____    __
 * |_______/   |________|  |________|  |________|  |__|
 *
 * Dice! QQ Dice Robot for TRPG
 * Copyright (C) 2018-2020 w4123���
 * Copyright (C) 2019-2020 String.Empty
 *
 * This program is free software: you can redistribute it and/or modify it under the terms
 * of the GNU Affero General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License along with this
 * program. If not, see <http://www.gnu.org/licenses/>.
 */
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <iostream>
#include <map>
#include <set>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <mutex>
#include <unordered_map>

#include "APPINFO.h"
#include "DiceFile.hpp"
#include "Jsonio.h"
#include "CQEVE_ALL.h"
#include "ManagerSystem.h"
#include "DiceMod.h"
#include "DiceMsgSend.h"
#include "MsgFormat.h"
#include "DiceCloud.h"
#include "CardDeck.h"
#include "DiceConsole.h"
#include "GlobalVar.h"
#include "CharacterCard.h"
#include "DiceEvent.h"
#include "DiceSession.h"
#include "DiceGUI.h"
#include "S3PutObject.h"

#pragma warning(disable:4996)
#pragma warning(disable:6031)

using namespace std;
using namespace CQ;

unordered_map<long long, User> UserList{};
ThreadFactory threads;
string strFileLoc;

constexpr auto msgInit{ R"(��ӭʹ��Dice!���������ˣ�
��Ӳ˵���Dice!��->���ۺϹ�������������ĺ�̨���
����Masterģʽͨ�������󼴿ɳ�Ϊ�ҵ�����~
�ɷ���.help�鿴����
�ο��ĵ��ο�.help����)" };

//��������
void loadData()
{
	mkDir(DiceDir);
	string strLog;
	loadDir(loadXML<CardTemp>, string(DiceDir + "\\CardTemp\\"), getmCardTemplet(), strLog, true);
	if (loadJMap(DiceDir + "\\conf\\CustomReply.json", CardDeck::mReplyDeck) < 0 && loadJMap(
		strFileLoc + "ReplyDeck.json", CardDeck::mReplyDeck) > 0)
	{
		console.log("Ǩ���Զ���ظ�" + to_string(CardDeck::mReplyDeck.size()) + "��", 1);
		saveJMap(DiceDir + "\\conf\\CustomReply.json", CardDeck::mReplyDeck);
	}
	fmt->set_help("�ظ��б�", "�ظ��������б�:" + listKey(CardDeck::mReplyDeck));
	if (loadDir(loadJMap, string(DiceDir + "\\PublicDeck\\"), CardDeck::mExternPublicDeck, strLog) < 1)
	{
		loadJMap(strFileLoc + "PublicDeck.json", CardDeck::mExternPublicDeck);
		loadJMap(strFileLoc + "ExternDeck.json", CardDeck::mExternPublicDeck);
	}
	map_merge(CardDeck::mPublicDeck, CardDeck::mExternPublicDeck);
	//��ȡ�����ĵ�
	fmt->load(strLog);
	if (int cnt; (cnt = loadJMap(DiceDir + "\\conf\\CustomHelp.json", CustomHelp)) < 0)
	{
		if (cnt == -1)console.log("�Զ�������ļ�json����ʧ�ܣ�", 1);
		ifstream ifstreamHelpDoc(strFileLoc + "HelpDoc.txt");
		if (ifstreamHelpDoc)
		{
			string strName, strMsg;
			while (getline(ifstreamHelpDoc, strName) && getline(ifstreamHelpDoc, strMsg))
			{
				while (strMsg.find("\\n") != string::npos)strMsg.replace(strMsg.find("\\n"), 2, "\n");
				while (strMsg.find("\\s") != string::npos)strMsg.replace(strMsg.find("\\s"), 2, " ");
				while (strMsg.find("\\t") != string::npos)strMsg.replace(strMsg.find("\\t"), 2, "	");
				CustomHelp[strName] = strMsg;
			}
			if (!CustomHelp.empty())
			{
				saveJMap(DiceDir + "\\conf\\CustomHelp.json", CustomHelp);
				console.log("��ʼ���Զ����������" + to_string(CustomHelp.size()) + "��", 1);
			}
		}
		ifstreamHelpDoc.close();
	}
	map_merge(fmt->helpdoc, CustomHelp);
	if (!strLog.empty())
	{
		strLog += "��չ���ö�ȡ��ϡ�";
		console.log(strLog, 1, printSTNow());
	}
}

//��ʼ��
void dataInit()
{
	gm = make_unique<DiceTableMaster>();
	if (gm->load() < 0)
	{
		multimap<long long, long long> ObserveGroup;
		loadFile(strFileLoc + "ObserveDiscuss.RDconf", ObserveGroup);
		loadFile(strFileLoc + "ObserveGroup.RDconf", ObserveGroup);
		for (auto [grp, qq] : ObserveGroup)
		{
			gm->session(grp).sOB.insert(qq);
			gm->session(grp).update();
		}
		ifstream ifINIT(strFileLoc + "INIT.DiceDB");
		if (ifINIT)
		{
			long long Group(0);
			int value;
			string nickname;
			while (ifINIT >> Group >> nickname >> value)
			{
				gm->session(Group).mTable["�ȹ�"].emplace(base64_decode(nickname), value);
				gm->session(Group).update();
			}
		}
		ifINIT.close();
		if(gm->mSession.size())console.log("��ʼ���Թ����ȹ���¼" + to_string(gm->mSession.size()) + "��", 1);
	}
	today = make_unique<DiceToday>(DiceDir + "/user/DiceToday.json");
}

//��������
void dataBackUp()
{
	mkDir(DiceDir + "\\conf");
	mkDir(DiceDir + "\\user");
	mkDir(DiceDir + "\\audit");
	//�����б�
	saveBFile(DiceDir + "\\user\\PlayerCards.RDconf", PList);
	saveFile(DiceDir + "\\user\\ChatList.txt", ChatList);
	saveBFile(DiceDir + "\\user\\ChatConf.RDconf", ChatList);
	saveFile(DiceDir + "\\user\\UserList.txt", UserList);
	saveBFile(DiceDir + "\\user\\UserConf.RDconf", UserList);
}

EVE_Enable(eventEnable)
{
	llStartTime = clock();
	char path[MAX_PATH];
	GetModuleFileNameA(nullptr, path, MAX_PATH);
	std::string pathStr(path);
	strModulePath = pathStr;
	string pathExe = pathStr.substr(pathStr.rfind("\\") + 1);
	std::transform(pathStr.begin(), pathStr.end(), pathStr.begin(), [](unsigned char c) { return tolower(c); });
	if (pathExe.substr(0, 4) == "java")
	{
		frame = QQFrame::Mirai;
		Dice_Full_Ver_For = Dice_Full_Ver + " For Mirai]";
		dirExe = pathStr.substr(0, pathStr.find("jre\\bin\\java.exe"));
		this_thread::sleep_for(3s); //ȷ��Mirai�첽��Ϣ����ִ�����
	}
	else if (pathExe.substr(0, 4) == "����") {
		frame = QQFrame::XianQu;
		dirExe = pathStr.substr(0, pathStr.find_last_of('\\') + 1);
	}
	{
		DiceDir = dirExe + "Dice" + to_string(getLoginQQ());
		filesystem::path pathDir(DiceDir);
		if (!exists(pathDir)) {
			filesystem::path pathDirOld(dirExe + "DiceData");
			if (exists(pathDirOld))rename(pathDirOld, pathDir);
			else filesystem::create_directory(pathDir);
		}
	}
	console.setPath(DiceDir + "\\conf\\Console.xml");
	strFileLoc = getAppDirectory();
	mkDir(strFileLoc); // Mirai�����Զ������ļ���
	console.DiceMaid = getLoginQQ();
	GlobalMsg["strSelfName"] = getLoginNick();
	if (GlobalMsg["strSelfName"].empty())
	{
		GlobalMsg["strSelfName"] = "����[" + toString(console.DiceMaid % 1000, 4) + "]";
	}
	mkDir(DiceDir + "\\conf");
	mkDir(DiceDir + "\\user");
	mkDir(DiceDir + "\\audit");
	if (!console.load())
	{
		ifstream ifstreamMaster(strFileLoc + "Master.RDconf");
		if (ifstreamMaster)
		{
			std::pair<int, int> ClockToWork{}, ClockOffWork{};
			int iDisabledGlobal, iDisabledMe, iPrivate, iDisabledJrrp, iLeaveDiscuss;
			ifstreamMaster >> console.masterQQ >> console.isMasterMode >> iDisabledGlobal >> iDisabledMe >> iPrivate >>
				iDisabledJrrp >> iLeaveDiscuss
				>> ClockToWork.first >> ClockToWork.second >> ClockOffWork.first >> ClockOffWork.second;
			console.set("DisabledGlobal", iDisabledGlobal);
			console.set("DisabledMe", iDisabledMe);
			console.set("Private", iPrivate);
			console.set("DisabledJrrp", iDisabledJrrp);
			console.set("LeaveDiscuss", iLeaveDiscuss);
			console.setClock(ClockToWork, ClockEvent::on);
			console.setClock(ClockOffWork, ClockEvent::off);
		}
		else
		{
			sendPrivateMsg(console.DiceMaid, msgInit);
		}
		ifstreamMaster.close();
		std::map<string, int> boolConsole;
		loadJMap(strFileLoc + "boolConsole.json", boolConsole);
		for (auto& [key, val] : boolConsole)
		{
			console.set(key, val);
		}
		console.setClock({ 11, 45 }, ClockEvent::clear);
		console.loadNotice();
		console.save();
	}
	//��ȡ�����б�
	if (loadBFile(DiceDir + "\\user\\UserConf.RDconf", UserList) < 1)
	{
		map<long long, int> DefaultDice;
		if (loadFile(strFileLoc + "Default.RDconf", DefaultDice) > 0)
			for (auto p : DefaultDice)
			{
				getUser(p.first).create(NEWYEAR).intConf["Ĭ����"] = p.second;
			}
		map<long long, string> DefaultRule;
		if (loadFile(strFileLoc + "DefaultRule.RDconf", DefaultRule) > 0)
			for (auto p : DefaultRule)
			{
				if (isdigit(static_cast<unsigned char>(p.second[0])))break;
				getUser(p.first).create(NEWYEAR).strConf["Ĭ�Ϲ���"] = p.second;
			}
		ifstream ifName(strFileLoc + "Name.dicedb");
		if (ifName)
		{
			long long GroupID = 0, QQ = 0;
			string name;
			while (ifName >> GroupID >> QQ >> name)
			{
				name = base64_decode(name);
				getUser(QQ).create(NEWYEAR).setNick(GroupID, name);
			}
		}
	}
	if (loadFile(DiceDir + "\\user\\UserList.txt", UserList) < 1)
	{
		set<long long> WhiteQQ;
		if (loadFile(strFileLoc + "WhiteQQ.RDconf", WhiteQQ) > 0)
			for (auto qq : WhiteQQ)
			{
				getUser(qq).create(NEWYEAR).trust(1);
			}
		//��ȡ����Ա�б�
		set<long long> AdminQQ;
		if (loadFile(strFileLoc + "AdminQQ.RDconf", AdminQQ) > 0)
			for (auto qq : AdminQQ)
			{
				getUser(qq).create(NEWYEAR).trust(4);
			}
		if (console.master())getUser(console.master()).create(NEWYEAR).trust(5);
		if (UserList.size())console.log("��ʼ���û���¼" + to_string(UserList.size()) + "��", 1);
	}
	if (loadBFile(DiceDir + "\\user\\ChatConf.RDconf", ChatList) < 1)
	{
		set<long long> GroupList;
		if (loadFile(strFileLoc + "DisabledDiscuss.RDconf", GroupList) > 0)
			for (auto p : GroupList)
			{
				chat(p).discuss().set("ͣ��ָ��");
			}
		GroupList.clear();
		if (loadFile(strFileLoc + "DisabledJRRPDiscuss.RDconf", GroupList) > 0)
			for (auto p : GroupList)
			{
				chat(p).discuss().set("����jrrp");
			}
		GroupList.clear();
		if (loadFile(strFileLoc + "DisabledMEGroup.RDconf", GroupList) > 0)
			for (auto p : GroupList)
			{
				chat(p).discuss().set("����me");
			}
		GroupList.clear();
		if (loadFile(strFileLoc + "DisabledHELPDiscuss.RDconf", GroupList) > 0)
			for (auto p : GroupList)
			{
				chat(p).discuss().set("����help");
			}
		GroupList.clear();
		if (loadFile(strFileLoc + "DisabledOBDiscuss.RDconf", GroupList) > 0)
			for (auto p : GroupList)
			{
				chat(p).discuss().set("����ob");
			}
		GroupList.clear();
		map<chatType, int> mDefault;
		if (loadFile(strFileLoc + "DefaultCOC.MYmap", mDefault) > 0)
			for (auto it : mDefault)
			{
				if (it.first.second == msgtype::Private)getUser(it.first.first)
				                                        .create(NEWYEAR).setConf("rc����", it.second);
				else chat(it.first.first).create(NEWYEAR).setConf("rc����", it.second);
			}
		if (loadFile(strFileLoc + "DisabledGroup.RDconf", GroupList) > 0)
			for (auto p : GroupList)
			{
				chat(p).group().set("ͣ��ָ��");
			}
		GroupList.clear();
		if (loadFile(strFileLoc + "DisabledJRRPGroup.RDconf", GroupList) > 0)
			for (auto p : GroupList)
			{
				chat(p).group().set("����jrrp");
			}
		GroupList.clear();
		if (loadFile(strFileLoc + "DisabledMEDiscuss.RDconf", GroupList) > 0)
			for (auto p : GroupList)
			{
				chat(p).group().set("����me");
			}
		GroupList.clear();
		if (loadFile(strFileLoc + "DisabledHELPGroup.RDconf", GroupList) > 0)
			for (auto p : GroupList)
			{
				chat(p).group().set("����help");
			}
		GroupList.clear();
		if (loadFile(strFileLoc + "DisabledOBGroup.RDconf", GroupList) > 0)
			for (auto p : GroupList)
			{
				chat(p).group().set("����ob");
			}
		GroupList.clear();
		map<long long, string> WelcomeMsg;
		if (loadFile(strFileLoc + "WelcomeMsg.RDconf", WelcomeMsg) > 0)
		{
			for (const auto& p : WelcomeMsg)
			{
				chat(p.first).group().setText("��Ⱥ��ӭ", p.second);
			}
		}
		if (loadFile(strFileLoc + "WhiteGroup.RDconf", GroupList) > 0)
		{
			for (auto g : GroupList)
			{
				chat(g).group().set("����ʹ��").set("����");
			}
		}
	}
	if (loadFile(DiceDir + "\\user\\ChatList.txt", ChatList) < 1)
	{
		map<chatType, time_t> mLastMsgList;
		for (auto it : mLastMsgList) 
		{
			if (it.first.second == msgtype::Private)getUser(it.first.first).create(it.second);
			else chat(it.first.first).create(it.second).lastmsg(it.second).isGroup = 2 - int(it.first.second);
		}
		std::map<long long, long long> mGroupInviter;
		if (loadFile(strFileLoc + "GroupInviter.RDconf", mGroupInviter) < 1)
		{
			for (auto it : mGroupInviter)
			{
				chat(it.first).group().inviter = it.second;
			}
		}
		if(ChatList.size())console.log("��ʼ��Ⱥ��¼" + to_string(ChatList.size()) + "��", 1);
	}
	for (auto& [gid,gname] : getGroupList())
	{
		chat(gid).group().name(gname).reset("����");
	}
	blacklist = make_unique<DDBlackManager>();
	if (blacklist->loadJson(DiceDir + "\\conf\\BlackList.json") < 0)
	{
		blacklist->loadJson(strFileLoc + "BlackMarks.json");
		int cnt = blacklist->loadHistory(strFileLoc);
		if (cnt) {
			blacklist->saveJson(DiceDir + "\\conf\\BlackList.json");
			console.log("��ʼ��������¼" + to_string(cnt) + "��", 1);
		}
	}
	else {
		blacklist->loadJson(DiceDir + "\\conf\\BlackListEx.json", true);
	}
	fmt = make_unique<DiceModManager>();
	if (loadJMap(DiceDir + "\\conf\\CustomMsg.json", EditedMsg) < 0)loadJMap(strFileLoc + "CustomMsg.json", EditedMsg);
	//Ԥ�޸ĳ����ظ��ı�
	if (EditedMsg.count("strSelfName"))GlobalMsg["strSelfName"] = EditedMsg["strSelfName"];
	for (auto it : EditedMsg)
	{
		while (it.second.find("��������") != string::npos)it.second.replace(it.second.find("��������"), 8,
		                                                                GlobalMsg["strSelfName"]);
		GlobalMsg[it.first] = it.second;
	}
	loadData();
	if (loadBFile(DiceDir + "\\user\\PlayerCards.RDconf", PList) < 1)
	{
		ifstream ifstreamCharacterProp(strFileLoc + "CharacterProp.RDconf");
		if (ifstreamCharacterProp)
		{
			long long QQ, GrouporDiscussID;
			int Type, Value;
			string SkillName;
			while (ifstreamCharacterProp >> QQ >> Type >> GrouporDiscussID >> SkillName >> Value)
			{
				if (SkillName == "����/���")SkillName = "����";
				getPlayer(QQ)[0].set(SkillName, Value);
			}
			console.log("��ʼ����ɫ����¼" + to_string(PList.size()) + "��", 1);
		}
		ifstreamCharacterProp.close();
	}
	for (const auto& pl : PList)
	{
		if (!UserList.count(pl.first))getUser(pl.first).create(NEWYEAR);
	}
	dataInit();
	// ȷ���߳�ִ�н���
	while (msgSendThreadRunning)Sleep(10);
	Aws::InitAPI(options);
	Enabled = true;
	threads(SendMsg);
	threads(ConsoleTimer);
	threads(warningHandler);
	threads(frqHandler);
	sch.start();
	console.log(GlobalMsg["strSelfName"] + "��ʼ����ɣ���ʱ" + to_string((clock() - llStartTime) / 1000) + "��", 0b1,
				printSTNow());
	//��������
	getDiceList();
	getExceptGroup();
	llStartTime = clock();
	return 0;
}

mutex GroupAddMutex;

bool eve_GroupAdd(Chat& grp)
{
	{
		unique_lock<std::mutex> lock_queue(GroupAddMutex);
		if (grp.lastmsg(time(nullptr)).isset("δ��") || grp.isset("����"))grp.reset("δ��").reset("����");
		else return false;
		if (ChatList.size() == 1 && !console.isMasterMode)sendGroupMsg(grp.ID, msgInit);
	}
	GroupInfo ginf(grp.ID);
	//Ⱥ��Ϣ�Ƿ��ȡ�ɹ�
	if (ginf.llGroup) {
		grp.Name = ginf.strGroupName;
	}
	else {
		ginf.llGroup = grp.ID;
	}
	if (grp.boolConf.empty() && ginf.nGroupSize > 499) {
		grp.set("Э����Ч");
	}
	if (!console["ListenGroupAdd"] || grp.isset("����"))return 0;
	long long fromGroup = grp.ID;
	string strNow = printSTNow();
	string strMsg(GlobalMsg["strSelfName"]);
	try 
	{
		strMsg += "�¼���:" + ginf.tostring();
		if (blacklist->get_group_danger(fromGroup)) 
		{
			grp.leave(blacklist->list_group_warning(fromGroup));
			strMsg += "Ϊ������Ⱥ������Ⱥ";
			console.log(strMsg, 0b10, printSTNow());
			return true;
		}
		if (grp.isset("ʹ������"))strMsg += "���ѻ�ʹ�����ɣ�";
		else if(grp.isset("Э����Ч"))strMsg += "��Ĭ��Э����Ч��";
		if (grp.inviter) {
			strMsg += ",������" + printQQ(chat(fromGroup).inviter);
		}
		console.log(strMsg, 0, strNow);
		int max_trust = 0;
		float ave_trust(0);
		//int max_danger = 0;
		long long ownerQQ = 0;
		ResList blacks;
		std::vector<GroupMemberInfo> list = getGroupMemberList(fromGroup);
		if (list.empty())
		{
			strMsg += "��ȺԱ����δ���أ�";
		}
		else 
		{
			int cntUser(0), cntMember(0);
			for (auto& each : list) 
			{
				if (each.QQID == console.DiceMaid)continue;
				cntMember++;
				if (UserList.count(each.QQID)) 
				{
					cntUser++;
					ave_trust += getUser(each.QQID).nTrust;
				}
				if (each.permissions > 1) 
				{
					max_trust |= (1 << trustedQQ(each.QQID));
					if (blacklist->get_qq_danger(each.QQID) > 1)
					{
						strMsg += ",���ֺ���������Ա" + printQQ(each.QQID);
						if (grp.isset("���"))
						{
							strMsg += "��Ⱥ��ڣ�";
						}
						else
						{
							sendGroupMsg(fromGroup, blacklist->list_qq_warning(each.QQID));
							grp.leave("���ֺ���������Ա" + printQQ(each.QQID) + "��Ԥ������Ⱥ");
							strMsg += "������Ⱥ";
							console.log(strMsg, 0b10, strNow);
							return true;
						}
					}
					if (each.permissions == 3)
					{
						ownerQQ = each.QQID;
						ave_trust += ginf.nGroupSize * trustedQQ(each.QQID);
						strMsg += "��Ⱥ��" + printQQ(each.QQID) + "��";
					}
					else
					{
						ave_trust += ginf.nGroupSize * trustedQQ(each.QQID) / 10;
					}
				}
				else if (blacklist->get_qq_danger(each.QQID) > 1)
				{
					//max_trust |= 1;
					blacks << printQQ(each.QQID);
					if (blacklist->get_qq_danger(each.QQID)) 
					{
						AddMsgToQueue(blacklist->list_self_qq_warning(each.QQID), fromGroup, msgtype::Group);
					}
				}
			}
			if (!chat(fromGroup).inviter && list.size() == 2 && ownerQQ)
			{
				chat(fromGroup).inviter = ownerQQ;
				strMsg += "������" + printQQ(ownerQQ);
			}
			if (!cntMember) 
			{
				strMsg += "��ȺԱ����δ���أ�";
			}
			else 
			{
				ave_trust /= cntMember;
				strMsg += "\n�û�Ũ��" + to_string(cntUser * 100 / cntMember) + "% (" + to_string(cntUser) + "/" + to_string(cntMember) + "), ���ζ�" + toString(ave_trust);
			}
		}
		if (!blacks.empty())
		{
			string strNote = "���ֺ�����ȺԱ" + blacks.show();
			strMsg += strNote;
		}
		if (console["Private"] && !grp.isset("����ʹ��"))
		{	
			//����СȺ�ƹ�����û���ϰ�����
			if (max_trust > 1 || ave_trust > 0.5)
			{
				grp.set("����ʹ��");
				strMsg += "\n���Զ�׷��ʹ������";
			}
			else 
			{
				strMsg += "\n������ʹ�ã�����Ⱥ";
				console.log(strMsg, 1, strNow);
				grp.leave(getMsg("strPreserve"));
				return true;
			}
		}
		if (!blacks.empty())console.log(strMsg, 0b10, strNow);
		else console.log(strMsg, 1, strNow);
	}
	catch (...)
	{
		console.log(strMsg + "\nȺ" + to_string(fromGroup) + "��Ϣ��ȡʧ�ܣ�", 0b1, printSTNow());
		return true;
	}
	if (grp.isset("Э����Ч")) return 0;
	if (!GlobalMsg["strAddGroup"].empty())
	{
		this_thread::sleep_for(2s);
		AddMsgToQueue(getMsg("strAddGroup"), {fromGroup, msgtype::Group});
	}
	if (console["CheckGroupLicense"] && !grp.isset("����ʹ��"))
	{
		grp.set("δ���");
		this_thread::sleep_for(2s);
		AddMsgToQueue(getMsg("strGroupLicenseDeny"), {fromGroup, msgtype::Group});
	}
	return false;
}

//��������ָ��

EVE_PrivateMsg_EX(eventPrivateMsg)
{
	if (!Enabled)return;
	FromMsg Msg(eve.message, eve.fromQQ);
	if (Msg.DiceFilter())eve.message_block();
}

EVE_GroupMsg_EX(eventGroupMsg)
{
	if (!Enabled)return;
	if (eve.isAnonymous())return;
	if (eve.isSystem())return;
	Chat& grp = chat(eve.fromGroup).group().lastmsg(time(nullptr));
	if (grp.isset("δ��") || grp.isset("����"))eve_GroupAdd(grp);
	if (!grp.isset("����"))
	{
		FromMsg Msg(eve.message, eve.fromGroup, msgtype::Group, eve.fromQQ);
		if (Msg.DiceFilter())eve.message_block();
	}
	if (grp.isset("������Ϣ"))eve.message_block();
}

EVE_DiscussMsg_EX(eventDiscussMsg)
{
	if (!Enabled)return;
	//time_t tNow = time(NULL);
	if (console["LeaveDiscuss"])
	{
		sendDiscussMsg(eve.fromDiscuss, getMsg("strLeaveDiscuss"));
		Sleep(1000);
		setDiscussLeave(eve.fromDiscuss);
		return;
	}
	Chat& grp = chat(eve.fromDiscuss).discuss().lastmsg(time(nullptr));
	if (blacklist->get_qq_danger(eve.fromQQ) && console["AutoClearBlack"])
	{
		const string strMsg = "���ֺ������û�" + printQQ(eve.fromQQ) + "���Զ�ִ����Ⱥ";
		console.log(printChat({eve.fromDiscuss, msgtype::Discuss}) + strMsg, 0b10, printSTNow());
		grp.leave(strMsg);
		return;
	}
	FromMsg Msg(eve.message, eve.fromDiscuss, msgtype::Discuss, eve.fromQQ);
	if (Msg.DiceFilter() || grp.isset("������Ϣ"))eve.message_block();
}

EVE_System_GroupMemberIncrease(eventGroupMemberIncrease)
{
	Chat& grp = chat(fromGroup);
	if (grp.isset("����"))return 0;
	if (beingOperateQQ != console.DiceMaid)
	{
		if (chat(fromGroup).strConf.count("��Ⱥ��ӭ"))
		{
			string strReply = chat(fromGroup).strConf["��Ⱥ��ӭ"];
			while (strReply.find("{at}") != string::npos)
			{
				strReply.replace(strReply.find("{at}"), 4, "[CQ:at,qq=" + to_string(beingOperateQQ) + "]");
			}
			while (strReply.find("{@}") != string::npos)
			{
				strReply.replace(strReply.find("{@}"), 3, "[CQ:at,qq=" + to_string(beingOperateQQ) + "]");
			}
			while (strReply.find("{nick}") != string::npos)
			{
				strReply.replace(strReply.find("{nick}"), 6, getStrangerInfo(beingOperateQQ).nick);
			}
			while (strReply.find("{age}") != string::npos)
			{
				strReply.replace(strReply.find("{age}"), 5, to_string(getStrangerInfo(beingOperateQQ).age));
			}
			while (strReply.find("{sex}") != string::npos)
			{
				strReply.replace(strReply.find("{sex}"), 5,
				                 getStrangerInfo(beingOperateQQ).sex == 0
					                 ? "��"
					                 : getStrangerInfo(beingOperateQQ).sex == 1
					                 ? "Ů"
					                 : "δ֪");
			}
			while (strReply.find("{qq}") != string::npos)
			{
				strReply.replace(strReply.find("{qq}"), 4, to_string(beingOperateQQ));
			}
			grp.update(time(nullptr));
			AddMsgToQueue(strReply, fromGroup, msgtype::Group);
		}
		if (blacklist->get_qq_danger(beingOperateQQ))
		{
			const string strNow = printSTNow();
			string strNote = printGroup(fromGroup) + "����" + GlobalMsg["strSelfName"] + "�ĺ������û�" + printQQ(
				beingOperateQQ) + "��Ⱥ";
			AddMsgToQueue(blacklist->list_self_qq_warning(beingOperateQQ), fromGroup, msgtype::Group);
			if (grp.isset("����"))strNote += "��Ⱥ���壩";
			else if (grp.isset("���"))strNote += "��Ⱥ��ڣ�";
			else if (grp.isset("Э����Ч"))strNote += "��ȺЭ����Ч��";
			else if (getGroupMemberInfo(fromGroup, console.DiceMaid).permissions > 1)strNote += "��Ⱥ����Ȩ�ޣ�";
			else if (console["LeaveBlackQQ"])
			{
				strNote += "������Ⱥ��";
				grp.leave("���ֺ������û�" + printQQ(beingOperateQQ) + "��Ⱥ,��Ԥ������Ⱥ");
			}
			console.log(strNote, 0b10, strNow);
		}
	}
	else
	{
		if (!grp.tLastMsg)grp.set("δ��");
		return eve_GroupAdd(grp);
	}
	return 0;
}

EVE_System_GroupMemberDecrease(eventGroupMemberDecrease)
{
	if (fromQQ == 0)return 0; // ����Mirai�ڻ�����������ȺʱҲ�����һ���������
	Chat& grp = chat(fromGroup);
	if (beingOperateQQ == console.DiceMaid)
	{
		grp.set("����");
		if (!console || grp.isset("����"))return 0;
		string strNow = printSTime(stNow);
		string strNote = printQQ(fromQQ) + "��" + printQQ(beingOperateQQ) + "�Ƴ���" + printChat(grp);
		console.log(strNote, 0b1000, strNow);
		if (!console["ListenGroupKick"] || trustedQQ(fromQQ) > 1 || grp.isset("���") || grp.isset("Э����Ч") || ExceptGroups.count(fromGroup)) return 0;
		DDBlackMarkFactory mark{fromQQ, fromGroup};
		mark.sign().type("kick").time(strNow).note(strNow + " " + strNote).comment(getMsg("strSelfCall") + "ԭ����¼");
		if (grp.inviter && trustedQQ(grp.inviter) < 2)
		{
			strNote += ";��Ⱥ�����ߣ�" + printQQ(grp.inviter);
			if (console["KickedBanInviter"])mark.inviterQQ(grp.inviter).note(strNow + " " + strNote);
		}
		grp.reset("����ʹ��").reset("����");
		blacklist->create(mark.product());
	}
	else if (mDiceList.count(beingOperateQQ) && subType == 2 && console["ListenGroupKick"])
	{
		if (!console || grp.isset("����"))return 0;
		string strNow = printSTime(stNow);
		string strNote = printQQ(fromQQ) + "��" + printQQ(beingOperateQQ) + "�Ƴ���" + printChat(grp);
		console.log(strNote, 0b1000, strNow);
		if (trustedQQ(fromQQ) > 1 || grp.isset("���") || grp.isset("Э����Ч") || ExceptGroups.count(fromGroup)) return 0;
		DDBlackMarkFactory mark{fromQQ, fromGroup};
		mark.type("kick").time(strNow).note(strNow + " " + strNote).DiceMaid(beingOperateQQ).masterQQ(mDiceList[beingOperateQQ]).comment(strNow + " " + printQQ(console.DiceMaid) + "Ŀ��");
		grp.reset("����ʹ��").reset("����");
		blacklist->create(mark.product());
	}
	return 0;
}

EVE_System_GroupBan(eventGroupBan)
{
	Chat& grp = chat(fromGroup);
	if (grp.isset("����") || (beingOperateQQ != console.DiceMaid && !mDiceList.count(beingOperateQQ)) || !console[
		"ListenGroupBan"])return 0;
	if (subType == 1)
	{
		if (beingOperateQQ == console.DiceMaid)
		{
			console.log(GlobalMsg["strSelfName"] + "��" + printGroup(fromGroup) + "�б��������", 0b10, printSTNow());
			return 1;
		}
	}
	else
	{
		string strNow = printSTNow();
		long long llOwner = 0;
		string strNote = "��" + printGroup(fromGroup) + "��," + printQQ(beingOperateQQ) + "��" + printQQ(fromQQ) + "����" + printDuringTime(duration);
		if (!console["ListenGroupBan"] || trustedQQ(fromQQ) > 1 || grp.isset("���") || grp.isset("Э����Ч") || ExceptGroups.count(fromGroup)) 
		{
			console.log(strNote, 0b10, strNow);
			return 1;
		}
		DDBlackMarkFactory mark{fromQQ, fromGroup};
		mark.type("ban").time(strNow).note(strNow + " " + strNote);
		if (mDiceList.count(fromQQ))mark.fromQQ(0);
		if (beingOperateQQ == console.DiceMaid)
		{
			if (!console)return 0;
			mark.sign().comment(getMsg("strSelfCall") + "ԭ����¼");
		}
		else
		{
			mark.DiceMaid(beingOperateQQ).masterQQ(mDiceList[beingOperateQQ]).comment(strNow + " " + printQQ(console.DiceMaid) + "Ŀ��");
		}
		//ͳ��Ⱥ�ڹ���
		int intAuthCnt = 0;
		string strAuthList;
		vector<GroupMemberInfo> list = getGroupMemberList(fromGroup);
		for (auto& member : list)
		{
			if (member.permissions == 3)
			{
				llOwner = member.QQID;
			}
			else if (member.permissions == 2)
			{
				strAuthList += '\n' + member.Nick + "(" + to_string(member.QQID) + ")";
				intAuthCnt++;
			}
		}
		strAuthList = "��Ⱥ��" + printQQ(llOwner) + ",���й���Ա" + to_string(intAuthCnt) + "��" + strAuthList;
		mark.note(strNow + " " + strNote);
		if (grp.inviter && beingOperateQQ == console.DiceMaid && trustedQQ(grp.inviter) < 2)
		{
			strNote += ";��Ⱥ�����ߣ�" + printQQ(grp.inviter);
			if (console["BannedBanInviter"])mark.inviterQQ(grp.inviter);
			mark.note(strNow + " " + strNote);
		}
		grp.reset("����").reset("����ʹ��").leave();
		console.log(strNote + strAuthList, 0b1000, strNow);
		blacklist->create(mark.product());
		return 1;
	}
	return 0;
}

EVE_Request_AddGroup(eventGroupInvited)
{
	if (!console["ListenGroupRequest"])return 0;
	if (subType == 2 && groupset(fromGroup, "����") < 1)
	{
		this_thread::sleep_for(3s);
		const string strNow = printSTNow();
		string strMsg = "Ⱥ�����������ԣ�" + printQQ(fromQQ) + ",Ⱥ:" +
			to_string(fromGroup) + "��";
		if (blacklist->get_group_danger(fromGroup)) 
		{
			strMsg += "\n�Ѿܾ���Ⱥ�ں������У�";
			console.log(strMsg, 0b10, strNow);
			setGroupAddRequest(responseFlag, 2, 2, "");
		}
		else if (blacklist->get_qq_danger(fromQQ))
		{
			strMsg += "\n�Ѿܾ����û��ں������У�";
			console.log(strMsg, 0b10, strNow);
			setGroupAddRequest(responseFlag, 2, 2, "");
		}
		else if (Chat& grp = chat(fromGroup).group(); grp.isset("����ʹ��")) {
			grp.set("δ��");
			grp.inviter = fromQQ;
			strMsg += "\n��ͬ�⣨Ⱥ������ʹ�ã�";
			console.log(strMsg, 1, strNow);
			setGroupAddRequest(responseFlag, 2, 1, "");
		}
		else if (trustedQQ(fromQQ))
		{
			grp.set("����ʹ��").set("δ��");
			grp.inviter = fromQQ;
			strMsg += "\n��ͬ�⣨�������û���";
			console.log(strMsg, 1, strNow);
			setGroupAddRequest(responseFlag, 2, 1, "");
		}
		else if (console && console["Private"])
		{
			sendPrivateMsg(fromQQ, getMsg("strPreserve"));
			strMsg += "\n�Ѿܾ�����ǰ��˽��ģʽ��";
			console.log(strMsg, 1, strNow);
			setGroupAddRequest(responseFlag, 2, 2, "");
		}
		else
		{
			grp.set("δ��");
			grp.inviter = fromQQ;
			strMsg += "��ͬ��";
			this_thread::sleep_for(2s);
			console.log(strMsg, 1, strNow);
			setGroupAddRequest(responseFlag, 2, 1, "");
		}
		return 1;
	}
	return 0;
}

EVE_Menu(eventMasterMode)
{
	if (console)
	{
		console.isMasterMode = false;
		console.killMaster();
		MessageBoxA(nullptr, "Masterģʽ�ѹرա�\nmaster�����", "Masterģʽ�л�", MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		console.isMasterMode = true;
		console.save();
		MessageBoxA(nullptr, "Masterģʽ�ѿ�����\n����������﷢��.master public/private", "Masterģʽ�л�", MB_OK | MB_ICONINFORMATION);
	}
	return 0;
}

EVE_Menu(eventGUI)
{
	return GUIMain();
}

void global_exit() {
	Enabled = false;
	threads.exit();
	threads = {};
	dataBackUp();
	sch.end();
	fmt.reset();
	gm.reset();
	PList.clear();
	ChatList.clear();
	UserList.clear();
	console.reset();
	EditedMsg.clear();
	blacklist.reset();
	Aws::ShutdownAPI(options);
}

EVE_Disable(eventDisable)
{
	global_exit();
	return 0;
}

EVE_Exit(eventExit)
{
	if (!Enabled)
		return 0;
	global_exit();
	return 0;
}

MUST_AppInfo_RETURN(CQAPPID);
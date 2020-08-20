/*
 *  _______     ________    ________    ________    __
 * |   __  \   |__    __|  |   _____|  |   _____|  |  |
 * |  |  |  |     |  |     |  |        |  |_____   |  |
 * |  |  |  |     |  |     |  |        |   _____|  |__|
 * |  |__|  |   __|  |__   |  |_____   |  |_____    __
 * |_______/   |________|  |________|  |________|  |__|
 *
 * Dice! QQ Dice Robot for TRPG
 * Copyright (C) 2018-2019 w4123���
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
#pragma once
#ifndef DICE_MSG_SEND
#define DICE_MSG_SEND
#include <string>
#include "CQMsgSend.h"

using chatType = std::pair<long long, CQ::msgtype>;
std::ifstream& operator>>(std::ifstream& fin, CQ::msgtype& t);
std::ifstream& operator>>(std::ifstream& fin, chatType& ct);
std::ofstream& operator<<(std::ofstream& fout, const chatType& ct);
/*
 *  ����������Ϣ������Ϣ���Ͷ���
 *  Param:
 *  const std::string& msg ��Ϣ�����ַ���
 *  long long target_id Ŀ��ID(QQ,Ⱥ�Ż�������uin)
 *  MsgType msg_type ��Ϣ����
 */
void AddMsgToQueue(const std::string& msg, long long target_id, CQ::msgtype msg_type = CQ::msgtype::Private);
void AddMsgToQueue(const std::string& msg, chatType ct);

/*
 * ��Ϣ�����̺߳���
 * ע��: ���������߳��е��ô˺���, �˺��������ڳ�ʼ����Ϣ�����߳�
 */
void SendMsg();
#endif /*DICE_MSG_SEND*/
#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>

using std::pair;
using std::string;
using std::to_string;
using std::vector;
using std::map;
using std::multimap;
using std::set;

class FromMsg;
class DiceTableMaster;

struct LogInfo{
	static constexpr auto dirLog{ "\\user\\log" };
	bool isLogging{ false };
	//����ʱ�䣬Ϊ0�򲻴���
	time_t tStart{ 0 };
	time_t tLastMsg{ 0 };
	string fileLog;
	//·�������棬��ʼ��ʱ����
	string pathLog;
	void update() {
		tLastMsg = time(nullptr);
	}
};

struct LinkInfo {
	bool isLinking{ false };
	string typeLink;
	//���󴰿ڣ�Ϊ0�򲻴���
	long long linkFwd{ 0 };
};

class DiceSession
{
	//��ֵ��
	map<string, map<string, int>> mTable;
	//�Թ���
	set<long long> sOB;
	//��־
	LogInfo logger;
	//����
	LinkInfo linker;
public:
	string type;
	//Ⱥ��
	long long room;

	DiceSession(long long group, string t = "simple") : room(group),type(t)
	{
		tUpdate = tCreate = time(nullptr);
	}

	friend void dataInit();
	friend class DiceTableMaster;

	//��¼����ʱ��
	time_t tCreate;
	//������ʱ��
	time_t tUpdate;

	DiceSession& create(time_t tt)
	{
		tCreate = tt;
		return *this;
	}

	DiceSession& update(time_t tt)
	{
		tUpdate = tt;
		return *this;
	}

	DiceSession& update()
	{
		tUpdate = time(nullptr);
		save();
		return *this;
	}

	[[nodiscard]] bool table_count(string key) const { return mTable.count(key); }
	int table_add(string, int, string);
	[[nodiscard]] string table_prior_show(string key) const;
	bool table_clr(string key);

	//�Թ�ָ��
	void ob_enter(FromMsg*);
	void ob_exit(FromMsg*);
	void ob_list(FromMsg*) const;
	void ob_clr(FromMsg*);
	[[nodiscard]] set<long long> get_ob() const { return sOB; }

	DiceSession& clear_ob()
	{
		sOB.clear();
		return *this;
	}
	
	//logָ��
	void log_new(FromMsg*);
	void log_on(FromMsg*);
	void log_off(FromMsg*);
	void log_end(FromMsg*);
	[[nodiscard]] string log_path()const;
	[[nodiscard]] bool is_logging() const { return logger.isLogging; }

	//linkָ��
	void link_new(FromMsg*);
	void link_start(FromMsg*);
	void link_close(FromMsg*);
	[[nodiscard]] bool is_linking() const { return linker.isLinking; }

	void save() const;
};

using Session = DiceSession;

class DiceFullSession : public DiceSession
{
	set<long long> sGM;
	set<long long> sPL;
};

class DiceTableMaster
{
public:
	map<long long, std::shared_ptr<Session>> mSession;
	Session& session(long long group);
	void session_end(long long group);
	//void save();
	int load();
};

inline std::unique_ptr<DiceTableMaster> gm;
inline set<long long>LogList;
//��ֹ�ŽӵȻ��ڲ���
inline map<long long, pair<long long,bool>>LinkList;
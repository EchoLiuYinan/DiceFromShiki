#pragma once
#include <string>

namespace CQ
{
	class CQstream
	{
	protected:
		std::string buf;
	public:
		virtual void clear() noexcept;

		//�ַ���
		virtual CQstream& append(const std::string& s) noexcept;
		virtual CQstream& operator <<(const std::string& s) noexcept;
		//
		virtual CQstream& append(const char* c) noexcept;
		virtual CQstream& operator <<(const char* c) noexcept;

		//����
		virtual CQstream& append(const int& i) noexcept;
		virtual CQstream& operator <<(const int& i) noexcept;
		virtual CQstream& append(const std::size_t& i) noexcept;
		virtual CQstream& operator <<(const std::size_t& i) noexcept;

		//������(Q��ʲô��)
		virtual CQstream& append(const long long& l) noexcept;
		virtual CQstream& operator <<(const long long& l) noexcept;


		//������Ʒ�
		virtual CQstream& operator <<(void (*control)(CQstream&));
		virtual void send() noexcept = 0;
		virtual void flush() noexcept;

		virtual ~CQstream() noexcept;
	};

	//���Ͳ����������
	void send(CQstream& log) noexcept;
	//ֻ����,����������,�´η���ʱ�������ظ�����
	void flush(CQstream& log) noexcept;
	void endl(CQstream& log) noexcept;
}

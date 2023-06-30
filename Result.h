#ifndef RESULT_H
#define RESULT_H

#include <string>
#include <typeinfo>

class Result
{
public:
	template <typename T>
	Result(bool bSuccess, const T* caller, const std::string& msg = "")
		: m_bIsSuccess(bSuccess), m_sMessage(msg)
	{
		const std::type_info& typeInfo = typeid(*caller);
		m_sClassName = typeInfo.name();
	}

	~Result();

	bool IsSuccess() const;
	const std::string& GetMessageString() const;
	const std::string& GetCaller() const;

private:
	bool			m_bIsSuccess;
	std::string		m_sMessage;
	std::string		m_sClassName;

};

#endif // !RESULT_H

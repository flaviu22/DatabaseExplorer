#pragma once
#include <afx.h>

#include <vector>

class CDBRecord : public CObject
{
public:
	std::vector<CString> m_arrValue{};
};

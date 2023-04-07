#pragma once

template<typename T>
class with_status:public T
{
public:
	typedef unsigned char status;
	namespace status
	{
		status okey = 0;
		status warning = 1;
		status error = 2;
		status fanal = 3;
	}
	status err_status;

	bool is_wrong()
	{
		return err_status != status::okey;
	}
	bool is_ok()
	{
		return err_status == status::okey;
	}
	bool is_error()
	{
		return err_status >= status::warning;
	}
};

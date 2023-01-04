#ifndef __OCTOMOVEMENT__H__
#define __OCTOMOVEMENT__H__

#define connectName "ens33"//网络名
#define octoPort 5000 

//指令令牌号码
#define headerAuthorization "Bearer 4554E6F2DB404CFA8511BF2F648F3755"

int is_number(char * s);/* Whether string s is a number. Returns 0 for non-number, 1 for integer, 2 for hex-integer, 3 for float */
bool decide_ext(const char *gname,const char *nsuff);//判断文件名后缀. gname: name of the given file, nsuff: suffix  you need


void get_netInfConnect(void);//初次连接
void scram(void);//急停


#endif


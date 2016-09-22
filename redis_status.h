#ifndef REDISSTATUS_H
#define REDISSTATUS_H
#include <string>
#include <hiredis/hiredis.h>
using std::string;
//redis server的返回值花样多,
//有时候返回给你nil,
//有时候返回空的array (既然为空那为什么不返回nil)
//有时候返回OK,
//有时候返回1或者0, (既然1也是代表ok的意思为什么不干脆把ok砍掉?)
//有时候返回ERROR
//我认为太过复杂了,所以我重新包装了返回值.
//你不需要太多的判断以及人肉记忆.

//成功执行ok标志为true, 返回为空则nil标志为true,
//可以从detail()看到出错具体错误信息
//可以从integer()得到返回的int类型数据
class RedisStatus
{
    friend class RedisContext;

public:
    RedisStatus();
    bool ok() const;
    bool nil() const;
    string detail() const;
    long long integer() const;
    redisReply* reply() const;

private:
    redisReply* reply_;
    bool ok_;
    bool nil_;
    string detailStr_;
    long long resultInt_;

};

#endif // REDISSTATUS_H

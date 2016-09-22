#include "redis_status.h"

RedisStatus::RedisStatus() : reply_(NULL), ok_(true),nil_(false), detailStr_(), resultInt_(0)
{

}

bool RedisStatus::ok() const
{
    return ok_;
}

bool RedisStatus::nil() const
{
    return nil_;
}

redisReply *RedisStatus::reply() const
{
    return reply_;
}

string RedisStatus::detail() const
{
    return detailStr_;
}

long long RedisStatus::integer() const
{
    return resultInt_;
}


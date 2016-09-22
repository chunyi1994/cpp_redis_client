#include <assert.h>
#include <iostream>

#include "redis_context.h"
#include "utils.h"

RedisContext::RedisContext() : conn_(NULL)
{

}

RedisContext::~RedisContext()
{
    if(conn_)
    {
        redisFree(conn_);
    }
}

RedisStatus RedisContext::connect(const string &ip, size_t port)
{
    if(conn_)
    {
        redisFree(conn_);
    }

    RedisStatus status;
    conn_ = redisConnect(ip.c_str(), port);
    if(!conn_)
    {
        status.ok_ = false;
    }

    return status;
}


RedisStatus RedisContext::commond(const string &cmd)
{
    assert(conn_ != NULL);
    RedisStatus status;
    status.reply_ =  (redisReply*)redisCommand(conn_, cmd.c_str());
    if(!status.reply_)
    {
        status.ok_ = false;
        return status;
    }
    //根据不同的type,填充不同返回值信息
    if(status.reply_->type == REDIS_REPLY_ERROR)
    {
        status.ok_ = false;
        status.detailStr_ = status.reply_->str;
    }
    else if(status.reply_->type == REDIS_REPLY_INTEGER)
    {
        status.resultInt_ = status.reply_->integer;
    }
    else if(status.reply()->type == REDIS_REPLY_STATUS)
    {
        status.detailStr_ =  status.reply_->str;
        if(status.detailStr_ == "OK")
        {
            status.resultInt_ = 1;
        }
    }
    else if(status.reply()->type ==  REDIS_REPLY_STRING)
    {

    }
    else if(status.reply()->type == REDIS_REPLY_NIL)
    {
        status.nil_ = true;
    }
    else if(status.reply()->type == REDIS_REPLY_ARRAY)
    {

    }

    return status;
}

RedisStatus RedisContext::set(const string &key, const string &value)
{
    assert(conn_ != NULL);
    string cmd = "SET " + key + " " + value;
    RedisStatus status = commond(cmd);
    return status;
}

//get成功则status.ok() == true 并且 status.nil() == false
//get失败可能是 status.ok() == false, 或者 status.nil() == true
//为了确保一定得到了想要的值, 请判断 if ( status.ok() && ! status.nil() )
RedisStatus RedisContext::get(const string &key, string &value)
{
    assert(conn_ != NULL);
    string cmd = "GET " + key;
    RedisStatus status = commond(cmd);
    value = "";
    if(status.reply_->type ==  REDIS_REPLY_STRING)
    {
        value = status.reply_->str;
    }
    return status;
}

//如果del的值本身就是不存在的,则status.integer() == 0
RedisStatus RedisContext::del(const string &key)
{
    assert(conn_ != NULL);
    string cmd = "DEL " + key;
    RedisStatus status = commond(cmd);
    return status;
}

RedisStatus RedisContext::incrby(const string &key, int amount)
{
    string cmd = "INCRBY " + key + " " + int2string(amount);
    RedisStatus status = commond(cmd);
    return status;
}

RedisStatus RedisContext::rpush(const string &listKey, const string &item)
{
    string cmd = "RPUSH " + listKey + " " + item;
    RedisStatus status = commond(cmd);
    return status;
}

RedisStatus RedisContext::lpush(const string &listKey, const string &item)
{
    string cmd = "LPUSH " + listKey + " " + item;
    RedisStatus status = commond(cmd);
    return status;
}

RedisStatus RedisContext::lrange(const string &listKey, int start, int end, vector<string> &items)
{
    string cmd = "LRANGE " + listKey + " " + int2string(start) + " " + int2string(end);
    return getVector(cmd, items);
}

RedisStatus RedisContext::lindex(const string &listKey, int index, string &item)
{
    string cmd = "LINDEX " + listKey + " " + int2string(index);
    RedisStatus status = commond(cmd);
    item = "";
    if(status.reply_->type == REDIS_REPLY_STRING)
    {
        item = status.reply_->str;
    }
    return status;
}

RedisStatus RedisContext::lpop(const string &listKey)
{
    string cmd = "LPOP " + listKey;
    RedisStatus status = commond(cmd);
    return status;
}

RedisStatus RedisContext::rpop(const string &listKey)
{
    string cmd = "RPOP " + listKey;
    RedisStatus status = commond(cmd);
    return status;
}

RedisStatus RedisContext::sadd(const string &setKey, const string &item)
{
    string cmd = "SADD " + setKey + " " + item;
    RedisStatus status = commond(cmd);
    return status;
}

RedisStatus RedisContext::smembers(const string &setKey, vector<string> &members)
{
    string cmd = "SMEMBERS " + setKey;
    return getVector(cmd, members);
}

RedisStatus RedisContext::sismember(const string &setKey, const string &item, bool &isMember)
{
    string cmd = "SISMEMBER " + setKey + " " + item;
    RedisStatus status = commond(cmd);
    if(status.reply_->type == REDIS_REPLY_INTEGER)
    {
        isMember = static_cast<bool>(status.reply_->integer);
    }
    else
    {
        isMember = false;
    }
    return status;
}

RedisStatus RedisContext::srem(const string &setKey, const string &item)
{
    string cmd = "SREM " + setKey + " " + item;
    RedisStatus status = commond(cmd);
    return status;
}

RedisStatus RedisContext::hget(const string &key, const string& field, string& value)
{
    string cmd = "HGET " + key + " " + field;
    RedisStatus status = commond(cmd);
    value = "";
    if(status.reply_->type == REDIS_REPLY_STRING)
    {
        value = status.reply_->str;
    }
    return status;
}


//redis server对set命令的返回是OK,不返回数字,
//hset, del之类的返回是integer数字
//redis server返回值还挺复杂的,难以记忆, 这里我对他做了简化工作
//任何一个有 set,del 语义的函数,都会返回一个integer,来表示多少个数据被成功的执行操作.
//可以通过status.integer()获得这个值,
//任何一个操作都可以通过status.ok() 来查看是否执行了
RedisStatus RedisContext::hset(const string &key, const string& field,const string& value)
{
    string cmd = "HSET " + key + " " + field + " " + value;
    RedisStatus status = commond(cmd);
    return status;
}

//请查看status.integer()
RedisStatus RedisContext::hdel(const string &key, const string &field)
{
    string cmd = "HDEL " + key + " " + field;
    RedisStatus status = commond(cmd);
    return status;
}

RedisStatus RedisContext::hlen(const string &key, long long &len)
{
    string cmd = "HLEN " + key;
    RedisStatus status = commond(cmd);
    if(status.reply_->type == REDIS_REPLY_INTEGER)
    {
        len = status.reply_->integer;
    }
    return status;
}

//redis server 对hgetall命令不会返回nil,
//如果真的是空的,也会返回空的array,以及提示信息(empty list or set)
//但是我认为这里应该返回nil提示.所以这里设计的和官方有一点点出入
//当hgetall收到空的array的时候,status.nil()会返回true.
//在本程序里面,任意一个带有get字样或者含有获取值的行为的函数,
//当成功执行但是收到空的内容时候,status.nil()都会返回true
//当执行失败, status.ok()会返回false
//为了确保值一定得到了,请用   if(status.ok() &&  !status.nil() )
//判断执行失败请用   if( !status.ok() || status.nil() ) { return GET_VALUE_FALURE; }
RedisStatus RedisContext::hgetall(const string &key, map<string, string>& values)
{
    assert(conn_ != NULL);
    values.clear();
    string cmd = "HGETALL " + key;
    RedisStatus status = commond(cmd);
    if(status.reply_->type == REDIS_REPLY_ARRAY)
    {
        for(size_t i = 1; i < status.reply_->elements; i = i + 2)
        {
            redisReply* keyReply = (redisReply*)(status.reply_->element[i - 1]);
            redisReply* valueReply = (redisReply*)(status.reply_->element[i]);
            string key = keyReply->str;
            string value = valueReply->str;
            values[key] = value;
        }
    }
    if(values.size() == 0)
    {
        status.nil_ = true;
    }

    return status;
}

//status.integer() == 1 表示yes, 0 表示no
RedisStatus RedisContext::hexists(const string &key, const string &field)
{
    string cmd = "HEXISTS " + key + " " + field;
    RedisStatus status = commond(cmd);
    return status;
}

RedisStatus RedisContext::hincrby(const string &key, const string &field, int increment)
{
    string cmd = "HINCRBY " + key + " " + field + " " + int2string(increment);
    RedisStatus status = commond(cmd);
    return status;
}

static const int OP_HKEYS = 1;
static const int OP_HVALS = 2;
RedisStatus RedisContext::getVector(const string &cmd, vector<string> &vec)
{
    assert(conn_ != NULL);
    vec.clear();
    RedisStatus status = commond(cmd);
    if(status.reply_->type == REDIS_REPLY_ARRAY)
    {
        for(size_t i = 0; i < status.reply_->elements; i++)
        {
            redisReply* reply = (redisReply*)(status.reply_->element[i]);
            string name = reply->str;
            vec.push_back(name);
        }
    }
    if(vec.size() == 0)
    {
        status.nil_ = true;
    }
    return status;
}

//同hgetall
RedisStatus RedisContext::hkeys(const string &key, vector<string> &keys)
{
    return getVector("HKEYS " + key, keys);
}

RedisStatus RedisContext::hvals(const string &key, vector<string> &values)
{
    return getVector("HVALS " + key, values);
}

RedisStatus RedisContext::zadd(const string &key, const int score, const string &member)
{
    string cmd = "ZADD " + key + " " + int2string(score) + " " + member;
    RedisStatus status = commond(cmd);
    return status;
}

//如果member或者key不存在,返回nil
RedisStatus RedisContext::zscore(const string &key, const string &member, int &score)
{
    string cmd = "ZSCORE " + key + " " + member;
    RedisStatus status = commond(cmd);
    if(status.reply_->type == REDIS_REPLY_STRING)
    {
        string value = status.reply_->str;
        score = string2int(value);
    }
    return status;
}

RedisStatus RedisContext::zrem(const string &key, const string &member)
{
    string cmd = "ZREM " + key + " " + member;
    RedisStatus status = commond(cmd);
    return status;
}

RedisStatus RedisContext::zcard(const string &key, long long &num)
{
    string cmd = "ZCARD " + key;
    RedisStatus status = commond(cmd);
    num = status.reply_->integer;
    return status;
}

RedisStatus RedisContext::zcount(const string &key, const int min, const int max, long long &count)
{
    string cmd = "ZCOUNT " + key + " " + int2string(min) + " " + int2string(max);
    RedisStatus status = commond(cmd);
    count = status.reply_->integer;
    return status;
}

RedisStatus RedisContext::zrank(const string &key, const string &member, long long &rank)
{
    string cmd = "ZRANK " + key + " " + member;
    RedisStatus status = commond(cmd);
    rank = status.reply_->integer;
    return status;
}

RedisStatus RedisContext::zincrby(const string &key, int increment, const string &member)
{
    string cmd = "ZINCRBY " + key + " " + int2string(increment) + " " + member;
    RedisStatus status = commond(cmd);
    return status;
}

RedisStatus RedisContext::zrange(const string &key, const int start, const int stop, vector<string> &members)
{
    string cmd = "ZRANGE " + key + " " + int2string(start) + " " + int2string(stop);
    return getVector(cmd, members);
}






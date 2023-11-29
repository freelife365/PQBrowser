#pragma once

#ifndef __SINGLETON_H__
#define __SINGLETON_H__

template <typename T>
class Singleton
{
public:
    static T& GetInstance();
    virtual ~Singleton(){};
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton) = delete;

protected:
    Singleton() {}
};

template <typename T>
T& Singleton<T>::GetInstance()
{
    static T instance
    {
    }
    return instance;
}

#endif // __SINGLETON_H__
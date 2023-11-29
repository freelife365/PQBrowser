#ifndef __NONCOPYABLE_H__
#define __NONCOPYABLE_H__

class NonCopyable
{
public:
    NonCopyable(NonCopyable const&) = delete;
    NonCopyable& operator=(NonCopyable const&) = delete;

protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
};

#endif // __NONCOPYABLE_H__

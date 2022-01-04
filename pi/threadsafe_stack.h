#include <stack>
#include <memory>
#include <mutex>
#include <condition_variable>

template<typename T>

class tsafe_stack{

private:
    mutable std::mutex mut;       
    std::stack<T> data_stack;
    std::condition_variable data_cond;

public:
    tsafe_stack(){}

    void push(T new_value){
        std::lock_guard<std::mutex> lk(mut);
        data_stack.push(new_value);
        data_cond.notify_one();
    }

    void wait_and_pop(T& value){
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk,[this]{return !data_stack.empty();});
        value=data_stack.front();
        data_stack.pop();
    }

    void wait_and_pop_all(T& value){
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk,[this]{return !data_stack.empty();});
        value=data_stack.top();
        while(!data_stack.empty()) data_stack.pop();
    }

    bool try_pop(T& value)    {
        std::lock_guard<std::mutex> lk(mut);
        if(data_stack.empty())
            return false;
        value=data_stack.front();
        data_stack.pop();
        return true;
    }

    bool empty() const{
        std::lock_guard<std::mutex> lk(mut);
        return data_stack.empty();
    }
};
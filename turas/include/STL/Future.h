//
// Created by liam_ on 7/4/2024.
//

#pragma once
#include <future>
#include <thread>

namespace turas {

    template<typename _Ty>
    using Future = std::future<_Ty>;

    template<typename _Ty, typename ... Args>
    constexpr Future<_Ty> Async(Args &&... args) {
        return std::async<_Ty>(std::forward<Args>(args)...);
    }

    template<typename _Ty>
    bool IsReady(Future<_Ty> const & o) {
        return o.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    }
}
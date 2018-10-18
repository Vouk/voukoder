#pragma once

#include <functional>

using namespace std;

template <typename T>
struct Callback;

template <typename Ret, typename... Params>
struct Callback<Ret(Params...)> {
	template <typename... Args>
	static Ret callback(Args... args) {
		return func(args...);
	}
	static function<Ret(Params...)> func;
};

template <typename Ret, typename... Params>
function<Ret(Params...)> Callback<Ret(Params...)>::func;
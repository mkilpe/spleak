#pragma once

#include <string_view>

namespace securepath {

void print_impl(std::string_view format);
void print_impl(void const*);
void print_impl(std::size_t);
inline void print_impl(char const* p) { print_impl(std::string_view(p)); }

template<typename... Args>
void print(std::string_view format, Args const&... args) {
	std::string_view::size_type pos = 0;

	//unused if args empty
	[[maybe_unused]] auto replace = [&](auto&& arg) {
			if(pos != std::string_view::npos) {
				auto f = format.find("{}", pos);
				if(f != std::string_view::npos) {
					print_impl(format.substr(pos, f-pos));
					print_impl(arg);
					pos = f+2;
				}
			}
		};

	(replace(args), ...);

	if(pos < format.size()) {
		print_impl(format.substr(pos));
	}
	print_impl("\n");
}

}

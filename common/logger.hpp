#pragma once

#include <string_view>

namespace securepath {

void print(std::string_view format);
/*
template<typename... Args>
void print(std::string_view format, Args const&... args) {
	std::string_view::size_type pos = 0;

	//unused if args empty
	[[maybe_unused]] auto replace = [&](auto&& arg) {
			if(pos != std::string_view::npos) {
				auto f = fmt.find("{}", pos);
				if(f != std::string_view::npos) {
					out << fmt.substr(pos, f-pos);
					out << arg;
					pos = f+2;
				}
			}
		};

	(replace(args), ...);

	if(pos < fmt.size()) {
		out << fmt.substr(pos);
	}

}
*/

}

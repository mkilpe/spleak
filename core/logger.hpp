#pragma once

#include "static_allocator.hpp"
#include "types.hpp"

namespace securepath::spleak  {

class logger;

void print(std::string_view);
void output(logger&, std::string_view s);

using log_vector = std::vector<char, static_allocator<char, static_alloc<4096>>>;

class logger {
public:
	logger() = default;
	logger(static_alloc<4096>& alloc) : buffer_(alloc) {}
	virtual ~logger() = default;

	template<typename... Args>
	void log(std::string_view format, Args const&... args) {
		std::string_view::size_type pos = 0;

		//unused if args empty
		[[maybe_unused]] auto replace = [&](auto&& arg) {
				if(pos != std::string_view::npos) {
					auto f = format.find("{}", pos);
					if(f != std::string_view::npos) {
						output(*this, format.substr(pos, f-pos));
						output(*this, arg);
						pos = f+2;
					}
				}
			};

		(replace(args), ...);

		if(pos < format.size()) {
			output(*this, format.substr(pos));
		}
		output(*this, std::string_view("\n"));
		do_print();
	}

	void add_to_buffer(std::string_view str) {
		buffer_.insert(buffer_.end(), str.begin(), str.end());
	}
protected:
	virtual void do_print() = 0;

protected:
	// buffer to format messages
	log_vector buffer_;
};

void output(logger&, void const*);
void output(logger&, std::uint64_t);
inline void output(logger& l, char const* p) { output(l, std::string_view(p)); }

struct console_logger final : public logger {
	using logger::logger;

	void do_print() override {
		print(std::string_view(buffer_.begin(), buffer_.end()));
		buffer_.clear();
	}
};

}

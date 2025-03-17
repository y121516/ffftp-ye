#pragma once
#include <Windows.h>
#include <string_view>

template<typename CharT>
HGLOBAL allocate_global_memory(std::basic_string_view<CharT> v) {
	const HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE, (v.size() + 1) * sizeof(CharT));
	if (!hMem) return nullptr;
	if (auto buf = static_cast<CharT*>(::GlobalLock(hMem))) {
		std::memcpy(buf, v.data(), v.size() * sizeof(CharT));
		buf[v.size()] = CharT();
		::GlobalUnlock(hMem);
		return hMem;
	}
	::GlobalFree(hMem);
	return nullptr;
}

inline HGLOBAL allocate_global_memory(std::string_view v) { return allocate_global_memory<char>(v); }
inline HGLOBAL allocate_global_memory(std::wstring_view v) { return allocate_global_memory<wchar_t>(v); }

class clipboard_guard {
	const bool opened_;
public:
	explicit clipboard_guard(HWND hWndNewOwner)
		: opened_(::OpenClipboard(hWndNewOwner))
	{
	}
	~clipboard_guard() {
		if (opened_) ::CloseClipboard();
	}
	bool is_open() const { return opened_; }
};

inline bool clipboard_set(
	_In_ UINT uFormat,
	_In_opt_ HANDLE hMem = nullptr,
	_In_opt_ HWND hWndNewOwner = nullptr)
{
	clipboard_guard guard(hWndNewOwner);
	if (!guard.is_open()) return false;
	if (!::EmptyClipboard()) return false;
	if (!::SetClipboardData(uFormat, hMem)) return false;
	return true;
}

inline bool copy_string_to_clipboard(const char* str, HWND hWnd)
{
	const HGLOBAL hMem = allocate_global_memory(str);
	if (!hMem) return false;
	if (!clipboard_set(CF_TEXT, hMem, hWnd)) {
		::GlobalFree(hMem);
		return false;
	}
	return true;
}

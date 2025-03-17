#include <Windows.h>
#include <expected>
#include <memory>
template<typename T, auto Deleter>
struct handle_deleter
{
	void operator()(T handle) const noexcept
	{
		if (handle) Deleter(handle);
	}
};

template<typename T, auto Deleter>
using unique_handle = std::unique_ptr<std::remove_pointer_t<T>, handle_deleter<T, Deleter>>;

using unique_registry_key = unique_handle<HKEY, ::RegCloseKey>;

std::expected<unique_registry_key, LSTATUS> registry_key_open(
	_In_ HKEY hKey,
	_In_ REGSAM samDesired,
	_In_opt_ LPCWSTR lpSubKey = nullptr,
	_In_opt_ DWORD ulOptions = 0
) noexcept
{
	HKEY result;
	LSTATUS status = ::RegOpenKeyExW(hKey, lpSubKey, ulOptions, samDesired, &result);
	if (status == ERROR_SUCCESS) return std::expected<unique_registry_key, LSTATUS>(std::in_place, result);
	else return std::unexpected<LSTATUS>(status);
}

std::expected<DWORD, LSTATUS> registry_value_get_dword(
	_In_ HKEY hKey,
	_In_opt_ LPCWSTR lpValueName = nullptr
) noexcept
{
	DWORD data;
	DWORD cbData = sizeof(data);
	LSTATUS status = ::RegQueryValueExW(hKey, lpValueName, nullptr, nullptr, reinterpret_cast<LPBYTE>(&data), &cbData);
	if (status == ERROR_SUCCESS) return data;
	else return std::unexpected(status);
}

std::expected<std::pair<unique_registry_key, DWORD>, LSTATUS> registry_key_create_with_disposition(
	_In_ HKEY hKey,
	_In_ LPCWSTR lpSubKey,
	_In_ DWORD dwOptions = REG_OPTION_NON_VOLATILE,
	_In_ REGSAM samDesired = 0,
	_In_opt_ CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes = nullptr,
	_In_opt_ LPWSTR lpClass = nullptr
)
{
	HKEY hkResult;
	DWORD dwDisposition;
	LSTATUS status = ::RegCreateKeyExW(hKey, lpSubKey, 0, lpClass, dwOptions, samDesired, lpSecurityAttributes, &hkResult, &dwDisposition);
	if (status == ERROR_SUCCESS) return std::make_pair(unique_registry_key{ hkResult }, dwDisposition);
	else return std::unexpected(status);
}

std::expected<unique_registry_key, LSTATUS> registry_key_create(
	_In_ HKEY hKey,
	_In_ LPCWSTR lpSubKey,
	_In_ DWORD dwOptions = REG_OPTION_NON_VOLATILE,
	_In_ REGSAM samDesired = 0,
	_In_opt_ CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes = nullptr,
	_In_opt_ LPWSTR lpClass = nullptr
)
{
	HKEY hkResult;
	LSTATUS status = ::RegCreateKeyExW(hKey, lpSubKey, 0, lpClass, dwOptions, samDesired, lpSecurityAttributes, &hkResult, nullptr);
	if (status == ERROR_SUCCESS) return std::expected<unique_registry_key, LSTATUS>{std::in_place, hkResult};
	else return std::unexpected(status);
}

LSTATUS	registry_key_delete(
	_In_ HKEY hKey,
	_In_ LPCWSTR lpSubKey
)
{
	return ::RegDeleteKeyW(hKey, lpSubKey);
}
#include <string_view>
LSTATUS	registry_key_delete(
	_In_ HKEY hKey,
	_In_ std::wstring_view subKey
)
{
	return ::RegDeleteKeyW(hKey, subKey.data());
}
// sizeof(DWORD)未満でもREG_DWORDいける気がする。要確認
inline
LSTATUS registry_value_set_dword(
	_In_ HKEY hKey,
	_In_opt_ LPCWSTR lpValueName,
	_In_ DWORD data
) noexcept
{
	return ::RegSetValueExW(hKey, lpValueName, 0, REG_DWORD, reinterpret_cast<const BYTE*>(&data), sizeof(data));
}

inline
LSTATUS registry_value_set_dword(
	_In_ HKEY hKey,
	_In_ DWORD data
) noexcept
{
	return registry_value_set_dword(hKey, nullptr, data);
}

template<std::size_t N>
inline
LSTATUS registry_value_set_sz(
	_In_ HKEY hKey,
	_In_opt_ LPCWSTR lpValueName,
	_In_reads_z_(N) const wchar_t(&data)[N]
) noexcept
{
	return ::RegSetValueExW(hKey, lpValueName, 0, REG_SZ, reinterpret_cast<const BYTE*>(&data), sizeof(data));
}

template<class T>
inline
LSTATUS registry_value_set_binary(
	_In_ HKEY hKey,
	_In_opt_ LPCWSTR lpValueName,
	_In_ const T& data
) noexcept
{
	return ::RegSetValueExW(hKey, lpValueName, 0, REG_BINARY, reinterpret_cast<const BYTE*>(&data), sizeof(data));
}

template<class T, std::size_t N>
inline
LSTATUS registry_value_set_binary(
	_In_ HKEY hKey,
	_In_opt_ LPCWSTR lpValueName,
	_In_reads_(length) const T* data,
	std::size_t length
) noexcept
{
	return ::RegSetValueExW(hKey, lpValueName, 0, REG_BINARY, reinterpret_cast<const BYTE*>(data), sizeof(data[0]) * length);
}

template<class T, std::size_t N>
inline
LSTATUS registry_value_set_binary(
	_In_ HKEY hKey,
	_In_opt_ LPCWSTR lpValueName,
	_In_reads_(N) const T(&data)[N]
) noexcept
{
	return ::RegSetValueExW(hKey, lpValueName, 0, REG_BINARY, reinterpret_cast<const BYTE*>(&data), sizeof(data[0]));
}

inline
LSTATUS registry_value_set_multi_sz(
	_In_ HKEY hKey,
	_In_opt_ LPCWSTR lpValueName,
	PCZZWSTR data,
	std::size_t length
) noexcept
{
	return ::RegSetValueExW(hKey, lpValueName, 0, REG_MULTI_SZ, reinterpret_cast<const BYTE*>(data), sizeof(data[0]) * length);
}

template<std::size_t N>
inline
LSTATUS registry_value_set_multi_sz(
	_In_ HKEY hKey,
	_In_opt_ LPCWSTR lpValueName,
	_NullNull_terminated_ wchar_t data[N]
) noexcept
{
	return registry_value_set_multi_sz(hKey, lpValueName, data, sizeof(data));
}


#if 0
DWORD refactored_LoadHideDriveListRegistry() noexcept
{
	constexpr LPCWSTR lpSubKey = LR"(Software\Microsoft\Windows\CurrentVersion\Policies\Explorer)";
	auto noDrives = registry_key_open(HKEY_CURRENT_USER, KEY_READ, lpSubKey)
		.and_then([](unique_registry_key key) noexcept { return registry_value_get_dword(key.get(), L"NoDrives"); })
		.value_or(0);
	return noDrives;
}
#endif

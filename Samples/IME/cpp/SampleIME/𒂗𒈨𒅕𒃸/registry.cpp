#include "𒂗𒈨𒅕𒃸/registry.h"

#include <algorithm>
#include <format>
#include <optional>
#include <set>
#include <string>

#include "wil/registry.h"

namespace 𒂗𒈨𒅕𒃸 {

LANGID GetTransientLangID() {
  auto const international_user_profile =
      wil::reg::open_unique_key(HKEY_CURRENT_USER,
                                LR"(Control Panel\International\User Profile)",
                                wil::reg::key_access::readwrite);
  constexpr PCWSTR 𒂗𒈨𒅕𒃸_tag = L"akk-Xsux";
  std::optional<wil::unique_hkey> 𒂗𒈨𒅕𒃸_language;
  std::optional<LANGID> 𒂗𒈨𒅕𒃸_langid;
  // List from
  // https://learn.microsoft.com/en-us/windows/win32/sysinfo/enumerating-registry-subkeys.
  std::set<LANGID> unused_transient_langids{0x2000,
                                            0x2400,
                                            0x2800,
                                            0x2C00,
                                            0x3000,
                                            0x3400,
                                            0x3800,
                                            0x3C00,
                                            0x4000,
                                            0x4400,
                                            0x4800,
                                            0x4C00};
  for (auto const& subkey :
       wil::make_range(wil::reg::key_iterator{international_user_profile.get()},
                       wil::reg::key_iterator{})) {
    auto const& language_tag = subkey.name;
    wil::unique_hkey language =
        wil::reg::open_unique_key(international_user_profile.get(),
                                  language_tag.c_str(),
                                  wil::reg::key_access::readwrite);
    if (auto const langid =
            wil::reg::try_get_value<DWORD>(language.get(), L"TransientLangId");
        langid.has_value()) {
      if (language_tag == 𒂗𒈨𒅕𒃸_tag) {
        𒂗𒈨𒅕𒃸_langid = static_cast<LANGID>(*langid);
      }
      unused_transient_langids.erase(static_cast<LANGID>(*langid));
    }
    if (language_tag == 𒂗𒈨𒅕𒃸_tag) {
      𒂗𒈨𒅕𒃸_language = std::move(language);
    }
  }
  if (!𒂗𒈨𒅕𒃸_langid.has_value()) {
    if (unused_transient_langids.empty()) {
      MessageBoxW(nullptr,
                  L"All transient LANGIDs in use",
                  nullptr,
                  MB_OK | MB_ICONERROR);
      return 0x2000;
    }
    𒂗𒈨𒅕𒃸_langid = *unused_transient_langids.begin();
  }
  std::wstring const 𒂗𒈨𒅕𒃸_langid_string = std::format(L"{:04X}", *𒂗𒈨𒅕𒃸_langid);
  std::wstring const 𒂗𒈨𒅕𒃸_padded_langid_string = L"0000" + 𒂗𒈨𒅕𒃸_langid_string;
  if (!𒂗𒈨𒅕𒃸_language.has_value()) {
    𒂗𒈨𒅕𒃸_language =
        wil::reg::create_unique_key(international_user_profile.get(),
                                    𒂗𒈨𒅕𒃸_tag,
                                    wil::reg::key_access::readwrite);
  }
  wil::reg::set_value<DWORD>(
      𒂗𒈨𒅕𒃸_language->get(), L"TransientLangId", *𒂗𒈨𒅕𒃸_langid);
  std::wstring const input_profile =
      𒂗𒈨𒅕𒃸_langid_string + L":{F87CB858-5A61-42FF-98E4-CF3966457808}";
  if (wil::reg::try_get_value<DWORD>(𒂗𒈨𒅕𒃸_language->get(),
                                     input_profile.c_str()) != 1) {
    wil::reg::set_value(𒂗𒈨𒅕𒃸_language->get(), input_profile.c_str(), 1);
  }
  auto languages = wil::reg::get_value<std::vector<std::wstring>>(
      international_user_profile.get(), L"Languages");
  if (std::find(languages.begin(), languages.end(), 𒂗𒈨𒅕𒃸_tag) ==
      languages.end()) {
    languages.emplace_back(𒂗𒈨𒅕𒃸_tag);
    wil::reg::set_value(
        international_user_profile.get(), L"Languages", languages);
  }
  MessageBoxW(
      nullptr,
      std::format(L"Registering with transient LANGID {:04X}", *𒂗𒈨𒅕𒃸_langid)
          .c_str(),
      nullptr,
      MB_OK | MB_ICONINFORMATION);
  auto const keyboard_layout_preload =
      wil::reg::open_unique_key(HKEY_CURRENT_USER,
                                LR"(Keyboard Layout\Preload)",
                                wil::reg::key_access::readwrite);
  int highest_index = 0;
  std::optional<int> 𒂗𒈨𒅕𒃸_preload_index;
  for (auto const& value :
       wil::make_range(wil::reg::value_iterator{keyboard_layout_preload.get()},
                       wil::reg::value_iterator{})) {
    std::string ascii_key;
    std::transform(value.name.begin(),
                   value.name.end(),
                   std::back_inserter(ascii_key),
                   [](wchar_t c) { return c <= 0x7F ? c : '?'; });
    int index;
    if (std::from_chars(
            ascii_key.data(), ascii_key.data() + ascii_key.size(), index)
            .ec == std::errc{}) {
      highest_index = std::max(highest_index, index);
      if (wil::reg::get_value<std::wstring>(keyboard_layout_preload.get(),
                                            value.name.c_str()) ==
          𒂗𒈨𒅕𒃸_padded_langid_string) {
        𒂗𒈨𒅕𒃸_preload_index = index;
      }
    }
  }
  if (!𒂗𒈨𒅕𒃸_preload_index.has_value()) {
    𒂗𒈨𒅕𒃸_preload_index = highest_index + 1;
    wil::reg::set_value(keyboard_layout_preload.get(),
                        std::to_wstring(*𒂗𒈨𒅕𒃸_preload_index).c_str(),
                        𒂗𒈨𒅕𒃸_padded_langid_string.c_str());
  }
  auto const keyboard_layout_substitutes =
      wil::reg::open_unique_key(HKEY_CURRENT_USER,
                                LR"(Keyboard Layout\Substitutes)",
                                wil::reg::key_access::readwrite);
  wil::reg::set_value(keyboard_layout_substitutes.get(),
                      𒂗𒈨𒅕𒃸_padded_langid_string.c_str(),
                      L"00000409");
  auto const hidden_dummy_layouts =
      wil::reg::open_unique_key(HKEY_CURRENT_USER,
                                LR"(Software\Microsoft\CTF\HiddenDummyLayouts)",
                                wil::reg::key_access::readwrite);
  wil::reg::set_value(hidden_dummy_layouts.get(),
                      𒂗𒈨𒅕𒃸_padded_langid_string.c_str(),
                      L"00000409");
  return *𒂗𒈨𒅕𒃸_langid;
}

}  // namespace 𒂗𒈨𒅕𒃸

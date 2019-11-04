#include "helpers.hpp"

#include <psapi.h>

namespace rh2
{
    hMod GetHostModule()
    {
        return GetModuleHandle(nullptr);
    }

    u64 GetBaseAddress(const hMod module)
    {
        return reinterpret_cast<uint64_t>(module ? module : GetHostModule());
    }

    u32 GetImageSize(const hMod module)
    {
        _MODULEINFO minfo;
        return GetModuleInformation(GetCurrentProcess(),
                                    static_cast<HMODULE>(module ? module : GetHostModule()), &minfo,
                                    sizeof(minfo))
                   ? minfo.SizeOfImage
                   : 0;
    }
} // namespace rh2

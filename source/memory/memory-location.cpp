#include "memory-location.hpp"

namespace rh2
{
    std::vector<ModifiedMemoryData> MemoryLocation::sm_modifiedLocations = {};

    void MemoryLocation::saveBytes(const size_t numBytes) const
    {
        ModifiedMemoryData data;

        data.loc              = m_location;
        data.numModifiedBytes = numBytes;
        data.origBytes        = new uint8_t[numBytes];

        memcpy(data.origBytes, as<void*>(), numBytes);

        sm_modifiedLocations.push_back(data);
    }

    void MemoryLocation::restore() const
    {
        for (auto it = sm_modifiedLocations.begin(); it != sm_modifiedLocations.end(); ++it)
        {
            if (m_location == it->loc.m_location)
            {
                const uint32_t oldProtect = 0;

                protect(it->numModifiedBytes, PAGE_EXECUTE_READWRITE, &oldProtect);
                memcpy(as<void*>(), it->origBytes, it->numModifiedBytes);
                protect(it->numModifiedBytes, oldProtect, nullptr);

                delete[] it->origBytes;
                it = sm_modifiedLocations.erase(it);

                if (it == sm_modifiedLocations.end())
                {
                    break;
                }
            }
        }
    }

    void MemoryLocation::RestoreAllModifiedBytes()
    {
        while (sm_modifiedLocations.size())
        {
            sm_modifiedLocations[0].loc.restore();
        }
    }

    bool MemoryLocation::protect(const size_t    size,
                                 const uint32_t  newProtect,
                                 const uint32_t* oldProtect) const
    {
        return VirtualProtect(as<void*>(), size, static_cast<DWORD>(newProtect),
                              reinterpret_cast<DWORD*>(&oldProtect)) == TRUE;
    }
} // namespace rh2

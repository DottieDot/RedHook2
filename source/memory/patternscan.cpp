#include "patternscan.hpp"

#include "helpers.hpp"

namespace rh2
{
    MemoryLocation PatternScan(const char* pattern, const char* mask, hMod module)
    {
        if (!module)
        {
            module = GetHostModule();
        }

        const auto   base    = reinterpret_cast<uint64_t>(module);
        auto         ptr     = reinterpret_cast<const char*>(base);
        const auto   end     = reinterpret_cast<const char*>(base + GetImageSize(module));
        const size_t ptrnLen = strlen(mask);
        size_t       byte    = 0;

        // __try block because you'll get an access violation before the end is reached
        __try
        {
            while (ptr < end)
            {
                // Check if bytes match or if it's a wildcard
                if (*ptr == pattern[byte] || mask[byte] == '?')
                {
                    ++byte;
                    ++ptr;

                    // Check if we have a full match
                    if (byte == ptrnLen)
                    {
                        return reinterpret_cast<uintptr_t>(ptr - ptrnLen);
                    }
                }
                else
                {
                    // Increment ptr by 1 if byte isn't 0
                    // This prevents skipping bytes
                    ptr += byte == 0;

                    byte = 0;
                }
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
        }

        return 0;
    }

    // Converts a byte string to a single byte
    // Example: "AB" -> '\xAB'
    char ByteStrToByte(const char chars[2])
    {
        uint8_t vals[2];

        for (int i = 0; i < 2; ++i)
        {
            vals[i] = chars[i] - '0' - (0x7 * (chars[i] > '9'));
        }

        return (vals[0] * 0x10) + vals[1];
    }

    MemoryLocation PatternScan(const char* pattern, const size_t len, const hMod module)
    {
        char sig[0xFF];
        char mask[0xFF];

        // Loop through pattern
        size_t j = 0;
        for (size_t i = 0; i < len;)
        {
            // Ignore whitespace
            if (pattern[i] == ' ')
                continue;

            // Check if byte isn't a wildcard
            if (pattern[i] != '?')
            {
                mask[j] = 'x';
                sig[j]  = ByteStrToByte(pattern + i);

                // Increment by 3 because an adjacend white space is expected
                i += 3;
            }
            else
            {
                mask[j] = '?';

                // Value doesn't matter because it's a wildcard
                sig[j] = '\x00';

                // Increment by 2 because an adjacend white space is expected
                // This also accomedates for ??
                i += 2;
            }
            ++j;
        }

        // Null terminate the mask
        mask[j] = '\0';

        // Actually search for the sig
        return PatternScan(sig, mask, module);
    }

    namespace literals
    {
        MemoryLocation operator""_Scan(const char* pattern, const size_t len)
        {
            return PatternScan(pattern, len);
        }
    } // namespace literals
} // namespace rh2

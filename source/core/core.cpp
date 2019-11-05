#include "core.hpp"

#include "../memory/patternscan.hpp"
#include "../hooking/detour-hook.hpp"
#include "../hooking/command-hook.hpp"
#include "../hooking/input-hook.hpp"

#include <memory>
#include <MinHook/MinHook.h>
#include <thread>
#include <chrono>
#include <fmt/format.h>
#include <unordered_map>

#include <fstream>

namespace rh2
{
    MemoryLocation g_PatchVectorResults;
    MemoryLocation g_s_CommandHash;
    MemoryLocation g_rage__scrThread__GetCmdFromHash;
    MemoryLocation g_rage__fwRandom__ms_Seed;

    std::unique_ptr<hooking::CommandHook> g_waitHook;
    std::unique_ptr<hooking::DetourHook>  g_rage__scrThread__RegisterCommand;

    Fiber g_gameFiber;

    bool InitializeHooks();
    bool InitializeCommandHooks();

    std::ofstream file;

    bool Init()
    {
        using namespace literals;
        using namespace std::chrono;
        using namespace std::chrono_literals;

        file.open("test.txt");

        // Wait for the game window, otherwise we can't do much
        auto timeout = high_resolution_clock::now() + 20s;
        while (!FindWindowA(nullptr, "Red Dead Redemption 2") &&
               high_resolution_clock::now() < timeout)
        {
            std::this_thread::sleep_for(100ms);
        }

        // Check if waiting for the window timed out
        if (high_resolution_clock::now() >= timeout)
        {
            return false;
        }

        file << "Window Found" << std::endl;

        // Find sigs
        MemoryLocation loc;
        if (loc = "8B 41 18 4C 8B C1 85 C0"_Scan)
            g_PatchVectorResults = loc;
        else
            return false;

        file << "PatchVectorResults Found" << std::endl;

        if (loc = "E8 ? ? ? ? 8B 9C F5 ? ? ? ?"_Scan)
            g_rage__scrThread__GetCmdFromHash = loc.get_call();
        else
            return false;

        file << "rage::scrThread::GetCmdFromhahs Found" << std::endl;

        if (loc = "E8 ? ? ? ? 8B 9C F5 ? ? ? ?"_Scan)
            s_CommandHash = g_s_CommandHash = loc.add(3).get_lea();
        else
            return false;

        file << "g_CommandHash Found" << std::endl;

        if (loc = "FF C8 48 89 15 ? ? ? ? 81 E2 ? ? ? ?"_Scan)
            g_rage__fwRandom__ms_Seed = loc.add(2).add(3).rip(4);
        else
            return false;

        file << "rage::fwRandom::ms_Seed Found "
             << fmt::format("0x{:016X}", g_rage__fwRandom__ms_Seed.as<uint64_t>()) << std::endl;

        file << "Sigs Found" << std::endl;

        auto st = MH_Initialize();
        if (st != MH_OK)
        {
            return false;
        }

        file << "Minhook Initialized" << std::endl;

        if (!InitializeHooks())
        {
            return false;
        }

        file << "Hooks initialized" << std::endl;

        // if (!hooking::input::InitializeHook())
        //{
        //  return false;
        //}

        file << "Input hook initialized" << std::endl;

        if (!InitializeCommandHooks())
        {
            return false;
        }

        file << "Initialized command hooks";

        while (!(*s_CommandHash))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        file << "Command table found" << std::endl;

        return true;
    }

    void Unload() {}

    std::vector<u64>             g_natives;
    std::unordered_map<u16, u16> aliases;
    u64                          My_rage__scrThread__RegisterCommand(void*         commandTable,
                                                                     NativeHash    hash,
                                                                     NativeHandler handler)
    {
        static u16 prev = 0xFFFF;
        static u8  v1   = 88u;

        u32 rnd = g_rage__fwRandom__ms_Seed.get<u32>();
        u32 ns = (unsigned int)(signed int)(float)((float)((float)(rnd & 0x7FFF) * 0.000030517578) *
                                                   (float)(v1 - 1));

        if (prev != ns)
        {
            prev = ns;
            --v1;

            ns = (unsigned int)(signed int)(float)((float)((float)(rnd & 0x7FFF) * 0.000030517578) *
                                                   (float)(v1 - 1));

            aliases[ns] = v1;
        }

        if (auto it = aliases.find(ns); it != aliases.end())
        {
            ns = it->second;
        }

        g_natives.push_back(hash);

        file << ns << std::endl;

        return g_rage__scrThread__RegisterCommand->orig<u64>(commandTable, hash, handler);
    }

    bool InitializeHooks()
    {
        using namespace literals;

        g_rage__scrThread__RegisterCommand = std::make_unique<hooking::DetourHook>(
            "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 20 0F B6 FA"_Scan,
            My_rage__scrThread__RegisterCommand);

        return                                              //
            g_rage__scrThread__RegisterCommand->enable() && //
            true;                                           //
    }

    void MyWait(rage::scrThread::Info* info)
    {
        static bool b = true;

        if (!g_gameFiber)
        {
            if (!(g_gameFiber = Fiber::ConvertThreadToFiber()))
            {
                g_gameFiber = Fiber::CurrentFiber();
            }
        }

        if (b)
        {
            b = false;

            std::ofstream file("natives.txt");
            for (const auto& native : g_natives)
            {
                file << fmt::format("0x{:016X}", native) << std::endl;
            }
        }

        // scripting::ScriptThreadManager::Update();
        file << "This comes from the sript thread..." << std::endl;

        g_waitHook->orig(info);
    }

    bool InitializeCommandHooks()
    {
        g_waitHook = std::make_unique<hooking::CommandHook>(
            0x4EDE34FBADD967A6ull, reinterpret_cast<NativeHandler>(MyWait));

        return                      //
            g_waitHook->enable() && //
            true;                   //
    }

    Fiber GetGameFiber()
    {
        return g_gameFiber;
    }

    MemoryLocation GetPatchVectorResults()
    {
        return g_PatchVectorResults;
    }

    MemoryLocation Get_s_CommandHash()
    {
        return g_s_CommandHash;
    }

    MemoryLocation Get_rage__scrThread__GetCmdFromHash()
    {
        return g_rage__scrThread__GetCmdFromHash;
    }
} // namespace rh2

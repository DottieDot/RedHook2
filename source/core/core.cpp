#include "core.hpp"

#include "../memory/patternscan.hpp"
#include "../hooking/detour-hook.hpp"
#include "../hooking/command-hook.hpp"
#include "../hooking/input-hook.hpp"
#include "../invoker/invoker.hpp"
#include "../scripting/script.hpp"

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

    std::unique_ptr<hooking::CommandHook> g_waitHook;

    Script*                              g_activeScript = nullptr;
    std::vector<std::pair<hMod, Script>> g_scripts;

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

        if (!hooking::input::InitializeHook())
        {
            return false;
        }

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

    bool InitializeHooks()
    {
        return true;
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

        for (auto& [_, script] : g_scripts)
        {
            g_activeScript = &script;
            script.update();
            g_activeScript = nullptr;
        }

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
    void ScriptRegister(hMod module, const Script& script)
    {
        g_scripts.push_back(std::pair(module, script));
    }

    void ScriptUnregister(hMod module)
    {
        for (auto it = g_scripts.begin(); it != g_scripts.end(); ++it)
        {
            if (it->first == module)
            {
                if (it = g_scripts.erase(it); it == g_scripts.end())
                {
                    break;
                }
            }
        }
    }

    void ScriptWait(const std::chrono::high_resolution_clock::duration& duration)
    {
        if (g_activeScript)
        {
            g_activeScript->wait(duration);
        }
    }
} // namespace rh2

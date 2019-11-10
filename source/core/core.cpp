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
#include <filesystem>
#include <mutex>
#include <atomic>
#include <unordered_set>

#include <fstream>

namespace rh2
{
    std::atomic_bool g_unloading = false;
    hMod             g_module;

    MemoryLocation g_PatchVectorResults;
    MemoryLocation g_s_CommandHash;
    MemoryLocation g_rage__scrThread__GetCmdFromHash;

    std::unique_ptr<hooking::CommandHook> g_waitHook;

    Script*                              g_activeScript = nullptr;
    std::vector<std::pair<hMod, Script>> g_scripts;
    std::unordered_set<hMod>             g_modules;

    std::mutex g_scriptMutex;

    Fiber g_gameFiber;

    bool InitializeHooks();
    bool InitializeCommandHooks();
    void LoadMods();

    bool Init(hMod module)
    {
        using namespace literals;
        using namespace std::chrono;
        using namespace std::chrono_literals;

        g_module = module;

        // Wait for the game window, otherwise we can't do much
        auto timeout = high_resolution_clock::now() + 20s;
        while (!FindWindowA("sgaWindow", "Red Dead Redemption 2") &&
               high_resolution_clock::now() < timeout)
        {
            std::this_thread::sleep_for(100ms);
        }
        std::this_thread::sleep_for(2s);

        // Check if waiting for the window timed out
        if (high_resolution_clock::now() >= timeout)
        {
            return false;
        }

        // Find sigs
        MemoryLocation loc;

        // PatchVectorResults
        if (loc = "8B 41 18 4C 8B C1 85 C0"_Scan)
            g_PatchVectorResults = loc;
        else
            return false;

        // rage::scrThread::GetCmdFromhash
        if (loc = "E8 ? ? ? ? 8B 9C F5 ? ? ? ?"_Scan)
        {
            g_rage__scrThread__GetCmdFromHash = loc.get_call();
            s_CommandHash = g_s_CommandHash = loc.get_call().add(3).get_lea();
        }
        else
            return false;

        auto st = MH_Initialize();
        if (st != MH_OK)
        {
            return false;
        }

        if (!InitializeHooks())
        {
            return false;
        }

        while (!(*s_CommandHash))
        {
            std::this_thread::sleep_for(100ms);
        }

        if (!hooking::input::InitializeHook())
        {
            return false;
        }

        if (!InitializeCommandHooks())
        {
            return false;
        }

        std::thread thrd(LoadMods);
        thrd.detach();

        return true;
    }

    void Unload()
    {
        using namespace std::chrono_literals;

        if (g_unloading)
            return;
        g_unloading = true;

        for (auto mod : g_modules)
        {
            FreeLibrary(static_cast<HMODULE>(mod));
        }

        if (!hooking::input::RemoveHook())
        {
            return;
        }

        if (!hooking::DisableHooks())
        {
            return;
        }

        if (!hooking::RemoveHooks())
        {
            return;
        }

        if (MH_Uninitialize() != MH_OK)
        {
            return;
        }

        MemoryLocation::RestoreAllModifiedBytes();

        FreeLibraryAndExitThread(static_cast<HMODULE>(g_module), 0);
    }

    bool InitializeHooks()
    {
        return true;
    }

    void MyWait(rage::scrThread::Info* info)
    {
        if (g_unloading)
            return g_waitHook->orig(info);

        if (!g_gameFiber)
        {
            if (!(g_gameFiber = Fiber::ConvertThreadToFiber()))
            {
                g_gameFiber = Fiber::CurrentFiber();
            }
        }

        // GET_HASH_OF_THIS_SCRIPT_NAME
        if (Invoker::Invoke<u32>(0xBC2C927F5C264960ull) == 0x27eb33d7u) // main
        {
            std::lock_guard _(g_scriptMutex);
            for (auto& [_, script] : g_scripts)
            {
                g_activeScript = &script;
                script.update();
                g_activeScript = nullptr;
            }
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
        std::lock_guard _(g_scriptMutex);
        g_modules.insert(module);
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

        g_modules.erase(module);
        FreeLibraryAndExitThread(static_cast<HMODULE>(module), 0);
    }

    void ScriptWait(const std::chrono::high_resolution_clock::duration& duration)
    {
        if (g_activeScript)
        {
            g_activeScript->wait(duration);
        }
    }

    void LoadMods()
    {
        using namespace std::filesystem;

        for (auto it = directory_iterator("scripts/"); it != directory_iterator(); ++it)
        {
            if (it->path().extension() == ".asi")
            {
                LoadLibraryW(it->path().wstring().c_str());
            }
        }
    }
} // namespace rh2

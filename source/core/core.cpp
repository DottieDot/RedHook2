#include "core.hpp"

#include "../memory/patternscan.hpp"
#include "../hooking/detour-hook.hpp"
#include "../hooking/command-hook.hpp"
#include "../hooking/input-hook.hpp"
#include "../invoker/invoker.hpp"
#include "../scripting/script.hpp"
#include "../logger/log-mgr.hpp"
#include "logs.hpp"

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
    MemoryLocation g_rage__scrProgram__sm_Globals;

    std::unique_ptr<hooking::CommandHook> g_waitHook;

    Script*                              g_activeScript = nullptr;
    std::vector<std::pair<hMod, Script>> g_scripts;
    std::unordered_set<hMod>             g_modules;

    std::mutex g_scriptMutex;

    Fiber g_gameFiber;

    bool InitializeHooks();
    bool InitializeCommandHooks();
    void LoadMods();
    void CreateLogs();

    bool Init(hMod module)
    {
        using namespace literals;
        using namespace std::chrono;
        using namespace std::chrono_literals;

        g_module = module;

        CreateLogs();

        logs::g_hLog->log("Initializing RedHook2");

        logs::g_hLog->log("Waiting for game window");

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
            logs::g_hLog->fatal("Timed out");
            return false;
        }
        logs::g_hLog->log("Game window found");

        logs::g_hLog->log("Searching patterns");

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

        if (loc = "4C 8D 05 ? ? ? ? 4D 8B 08 4D 85 C9 74 11")
            rage::scrProgram::sm_Globals = g_rage__scrProgram__sm_Globals = loc.get_lea();
        else
            false;

        logs::g_hLog->log("Patterns found");

        logs::g_hLog->log("Initializing Minhook");
        auto st = MH_Initialize();
        if (st != MH_OK)
        {
            logs::g_hLog->log("Minhook failed to initialize {} ({})", MH_StatusToString(st), st);
            return false;
        }
        logs::g_hLog->log("Minhook initialized");

        logs::g_hLog->log("Initializing hooks");
        if (!InitializeHooks())
        {
            return false;
        }
        logs::g_hLog->log("Hooks initialized");

        logs::g_hLog->log("Waiting for natives");
        while (!(*s_CommandHash))
        {
            std::this_thread::sleep_for(100ms);
        }
        logs::g_hLog->log("Natives loaded");

        logs::g_hLog->log("Initializing input hook");
        if (!hooking::input::InitializeHook())
        {
            return false;
        }
        logs::g_hLog->log("Input hook initialized");

        logs::g_hLog->log("Initializing native hooks");
        if (!InitializeCommandHooks())
        {
            return false;
        }
        logs::g_hLog->log("Natives initialized");

        logs::g_hLog->log("RedHook2 initialized");

        logs::g_hLog->log("Loading mods");
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

        logs::g_hLog->log("Unloaded {} mods", g_modules.size());
        while (!g_modules.empty())
        {
            FreeLibrary(static_cast<HMODULE>(*g_modules.begin()));
        }
        logs::g_hLog->log("Scripts unloaded");

        logs::g_hLog->log("Removing input hook");
        if (!hooking::input::RemoveHook())
        {
            logs::g_hLog->fatal("Failed to remove input hook");
            return;
        }
        logs::g_hLog->log("Input hook removed");

        logs::g_hLog->log("Removing hooks");
        if (!hooking::DisableHooks())
        {
            logs::g_hLog->fatal("Failed to disable hooks");
            return;
        }
        logs::g_hLog->log("Hooks disabled");
        if (!hooking::RemoveHooks())
        {
            logs::g_hLog->fatal("Failed to remove hooks");
            return;
        }
        logs::g_hLog->log("Hooks removed");

        logs::g_hLog->log("Uninitializing Minhook");
        auto st = MH_Uninitialize();
        if (st != MH_OK)
        {
            logs::g_hLog->fatal("Failed to unitialized Minhook {} ({})", MH_StatusToString(st), st);
            return;
        }
        logs::g_hLog->log("Minhook uninitialized");

        logs::g_hLog->log("Restoring memory");
        MemoryLocation::RestoreAllModifiedBytes();
        logs::g_hLog->log("Memory restored");

        logs::g_hLog->log("RedHook2 unloaded");

        logging::LogMgr::DeleteAllLogs();

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

    void CreateLogs()
    {
        std::filesystem::create_directories("RedHook2/logs");

        logs::g_hLog = logging::LogMgr::CreateLog<logging::GenericFileLogger>(
            "hook_log", "RedHook2/logs/hook.log");
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
        g_scripts.push_back(std::pair(module, script));
        logs::g_hLog->log("Script registred by {}", module);
    }

    void ScriptUnregister(hMod module)
    {
        logs::g_hLog->log("Unloading module {}", module);
        i32 numScripts = 0;
        for (auto it = g_scripts.begin(); it != g_scripts.end(); ++it)
        {
            if (it->first == module)
            {
                ++numScripts;
                if (it = g_scripts.erase(it); it == g_scripts.end())
                {
                    break;
                }
            }
        }

        g_modules.erase(module);
        logs::g_hLog->log("Module {} unloaded with {} scripts", module, numScripts);
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
                auto  name   = it->path().filename().string();
                void* module = LoadLibraryW(it->path().wstring().c_str());
                if (module)
                {
                    g_modules.insert(module);
                    logs::g_hLog->log("Loaded {} (handle: {})", name, module);
                }
                else
                {
                    logs::g_hLog->log("Failed to load {} ({:X})", name, GetLastError());
                }
            }
        }

        logs::g_hLog->log("Mods loaded");
    }
} // namespace rh2

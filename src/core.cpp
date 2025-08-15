#include "define.hpp"
#include "core.hpp"
#include "io.hpp"
#include "i18n.hpp"

void CORE::SignalHandler(int signum)
{
    DIE(t("received_signal") + " " + std::to_string(signum));
}

BOOL CORE::IsAdmin()
{
    BOOL IsAdmin = FALSE;
    PSID AdminGroup = NULL;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (!AllocateAndInitializeSid(
            &NtAuthority,
            2,
            SECURITY_BUILTIN_DOMAIN_RID,
            DOMAIN_ALIAS_RID_ADMINS,
            0, 0, 0, 0, 0, 0,
            &AdminGroup))
        DIE(t("failed_allocate_sid"));
    if (!CheckTokenMembership(NULL, AdminGroup, &IsAdmin))
        DIE(t("failed_check_token"));
    FreeSid(AdminGroup);
    return IsAdmin;
}

void CORE::BindSignal()
{
    IO::Debug(t("binding_signal_handlers"));
    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);
    signal(SIGABRT, SignalHandler);
    IO::Debug(t("signal_handlers_bound"));
}

void CORE::Header()
{
    IO::Info(t("paper"));
    IO::Info("");
}

void CORE::ElevateNow()
{
    IO::Debug(t("checking_admin_privileges"));
    if (!IsAdmin())
    {
        IO::Debug(t("not_admin_requesting_elevation"));
        if (!IO::Confirm(t("admin_privileges_required")))
            DIE(t("user_declined_elevation"));
        TCHAR ModulePath[MAX_PATH];
        if (!GetModuleFileName(NULL, ModulePath, ARRAYSIZE(ModulePath)))
            DIE(t("failed_get_module_name"));
        IO::Debug(t("attempting_elevation"));
        SHELLEXECUTEINFO ShellExecuteInfo = {sizeof(ShellExecuteInfo)};
        ShellExecuteInfo.lpVerb = TEXT("runas");
        ShellExecuteInfo.lpFile = ModulePath;
        ShellExecuteInfo.hwnd = NULL;
        ShellExecuteInfo.nShow = SW_SHOWDEFAULT;
        if (!ShellExecuteEx(&ShellExecuteInfo))
            DIE(t("failed_elevate_privileges"));
        IO::Debug(t("elevation_request_sent"));
        exit(0);
    }
    else
    {
        IO::Debug(t("already_admin"));
    }
}

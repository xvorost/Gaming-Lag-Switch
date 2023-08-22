#pragma once

#include "ImGui/imgui.h"

struct Log
{
    ImGuiTextBuffer     Buf;
    bool                ScrollToBottom;

    void    Clear() { Buf.clear(); }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        va_list args;
        va_start(args, fmt);
        Buf.appendfv(fmt, args);
        va_end(args);
        ScrollToBottom = true;
    }

    void    Draw(const char* title, bool* p_opened = NULL)
    {
        ImGui::BulletText(title);
        ImGui::TextUnformatted(Buf.begin());
        if (ScrollToBottom)
            ImGui::SetScrollHereY(1.0f);
        ScrollToBottom = false;
    }
};
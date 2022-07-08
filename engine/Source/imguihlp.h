/* -*- c++ -*- */
#pragma once

#include "imgui/imgui.h"
#include "engine_tier0/DLL.h"

namespace engine
{

namespace imgui
{

_ENGINE_DLLEXP ImGuiContext* SetupImGui();
_ENGINE_DLLEXP void SetCurrentContext(ImGuiContext* _Ctx);
_ENGINE_DLLEXP void SetAllocProcs();

} // namespace imgui

} // namespace engine

#include "imguihlp.h"
#include <string>
#include <tier0lib/Win32.h>

static void* _Inner_Alloc(size_t _Size, void* __Heap)
{
	HANDLE _Heap = (HANDLE)__Heap;
	return HeapAlloc(_Heap, 0, _Size);
}

static void _Inner_Free(void* _Ptr, void* __Heap)
{
	HANDLE _Heap = (HANDLE)__Heap;
	HeapFree(_Heap, 0, _Ptr);
}

ImGuiContext* engine::imgui::SetupImGui()
{
	ImGuiContext* ct = ImGui::CreateContext();
	ImGui::StyleColorsClassic();
	engine::imgui::SetCurrentContext(ct);
	engine::imgui::SetAllocProcs();
	return ct;
}

void engine::imgui::SetAllocProcs()
{
	ImGui::SetAllocatorFunctions(_Inner_Alloc, _Inner_Free,
		(void*)GetProcessHeap());
}

void engine::imgui::SetCurrentContext(ImGuiContext* _Ctx)
{
	ImGui::SetCurrentContext(_Ctx);
}

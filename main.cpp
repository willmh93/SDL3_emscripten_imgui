#include <iostream>

#define SDL_MAIN_HANDLED
#include <GLES3/gl3.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_touch.h>

#include <emscripten.h>
#include <emscripten/html5.h>

#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_opengl3.h"

#include "emscripten_browser_clipboard.h"

SDL_Window* window = nullptr;

std::string clipboard_content;
bool simulatedImguiPaste = false;

char const* get_content_for_imgui(ImGuiContext*)
{
    std::cout << "ImGui requested clipboard content, returning " << clipboard_content.c_str() << std::endl;
    return clipboard_content.c_str();
}

void set_content_from_imgui(ImGuiContext*, char const* text)
{
    /// Callback for imgui, to set clipboard content
    clipboard_content = text;
	
	// send clipboard data to the browser
    emscripten_browser_clipboard::copy(clipboard_content); 
}

void clipboard_paste_callback(std::string&& paste_data, void* callback_data)
{
    std::cout << "Copied clipboard data: " << paste_data.c_str() << std::endl;
	
    // Set internal clipboard content
    clipboard_content = std::move(paste_data);

    // Simulate Ctrl+V keypress for ImGui
    ImGui::GetIO().AddKeyEvent(ImGuiKey_ModCtrl, true);
    ImGui::GetIO().AddKeyEvent(ImGuiKey_V, true);
    simulatedImguiPaste = true;
}

void gui_loop()
{
    // ======== Poll SDL events ========
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        ImGui_ImplSDL3_ProcessEvent(&e);
    }

    // ======== Prepare frame ========
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // ======== Draw window ========
    static bool demo_open = true;
	ImGui::ShowDemoWindow(&demo_open);


    // ======== Render ========
    ImGui::Render();
    glClearColor(0.1f, 0.0f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);

    // Release simulated paste keys
	if (simulatedImguiPaste) {
		simulatedImguiPaste = false;
		ImGui::GetIO().AddKeyEvent(ImGuiKey_ModCtrl, false);
		ImGui::GetIO().AddKeyEvent(ImGuiKey_V, false);
	}
}

int main()
{
	// ======== SDL Window setup ========
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 0);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	int fb_w = 1280, fb_h = 720;
	emscripten_get_canvas_element_size("#canvas", &fb_w, &fb_h);

	window = SDL_CreateWindow("my_app", fb_w, fb_h, 
		SDL_WINDOW_OPENGL | 
		SDL_WINDOW_RESIZABLE | 
		SDL_WINDOW_MAXIMIZED | 
		SDL_WINDOW_HIGH_PIXEL_DENSITY);

	if (!window)
	{
	    std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
	    return 1;
	}

	// ======== OpenGL setup ========
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);


	// ======== ImGui setup ========
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplSDL3_InitForOpenGL(window, gl_context);

	// Set paste callback
	emscripten_browser_clipboard::paste(clipboard_paste_callback);

    // Set clipboard callbacks for ImGui
	ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
	platform_io.Platform_GetClipboardTextFn = get_content_for_imgui;
	platform_io.Platform_SetClipboardTextFn = set_content_from_imgui;

	// Only stop propagation for Ctrl-V
	EM_ASM({
	    window.addEventListener('keydown', function(event) {
	        if (event.ctrlKey && event.key == 'v')
	            event.stopImmediatePropagation();
	    }, true);
	});

	ImGui_ImplOpenGL3_Init();


	// ======== Start main gui loop ========
    emscripten_set_main_loop(gui_loop, 0, true);


	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
	SDL_GL_DestroyContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
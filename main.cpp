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

SDL_Window* window = nullptr;

void gui_loop()
{
    // ======== Poll SDL events ========
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        ImGui_ImplSDL3_ProcessEvent(&e);

        //switch (e.type)
        //{
        //    case SDL_EVENT_QUIT: shared_sync.quit(); break;
        //    case SDL_EVENT_WINDOW_RESIZED:
        //    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        //        Platform()->resized();
        //        break;
        //    default: ProjectWorker::instance()->queueEvent(e); break;
        //}
    }

    //Platform()->update();

    // ======== Prepare frame ========
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    //ImGui::GetIO().DisplaySize = ImVec2((float)Platform()->fbo_width(), (float)Platform()->fbo_height());
    ImGui::GetIO().DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
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
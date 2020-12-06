// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

constexpr size_t INPUT_OUTPUT_BUFFER_SIZE = 9999;
constexpr size_t GRAPH_SIZE = 100;
constexpr const char* const BUTTONS[] = { "7", "8", "9", "*", "(", ")", "log", 
                                             "4", "5", "6", "/", "sin", "arcsin", "ln",
                                             "1", "2", "3", "%", "cos", "arccos", "lb",
                                              "0", ".", "-", "+", "tan", "arctan", "sqrt" };

#include "EEvaluator.hpp"
#include "implot/implot.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include "UtilityImCalc.hpp"

// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>            // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>            // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>          // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
#include <glad/gl.h>            // Initialize with gladLoadGL(...) or gladLoaderLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h>  // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h>// Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>
#include <vector>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
EEvaluator evaluator;
static std::ostringstream graph_variable;
static char* buffer;
static float graphX[GRAPH_SIZE], graphY[GRAPH_SIZE];
static ImVec4 error_font_color = ImVec4(1, 0, 0, 1);
static bool f_of_y = false;
static bool f_of_x = true;
static std::string graph_error;

static void fillGraph() {
    if (f_of_y) {
        try {
            for (long i = 0; i < GRAPH_SIZE; i++) {
                graph_variable.str("");
                graph_variable << "(Y = " << std::setprecision(5) << ((i - 50) / 10.0f) << ")";
                auto initVariables = graph_variable.str();
                evaluator.evaluateFloat(initVariables.c_str(), initVariables.length());
                graph_variable.str("");
                graph_variable << "(y = " << std::setprecision(5) << ((i - 50) / 10.0f) << ")";
                initVariables = graph_variable.str();
                evaluator.evaluateFloat(initVariables.c_str(), initVariables.length());
                graphX[i] = evaluator.evaluateFloat(buffer, INPUT_OUTPUT_BUFFER_SIZE);
                graphY[i] = ((i - 50) * 0.1f);
            }
            graph_error.assign("");
        }
        catch (std::exception const& e) {
            graph_error.assign(e.what());
        }
    }
    else {
        try {
            for (long i = 0; i < GRAPH_SIZE; i++) {
                graph_variable.str("");
                graph_variable << "(X = " << std::setprecision(5) << ((i - 50) * 0.1f) << ")";
                auto initVariables = graph_variable.str();
                evaluator.evaluateFloat(initVariables.c_str(), initVariables.length());
                graph_variable.str("");
                graph_variable << "(x = " << std::setprecision(5) << ((i - 50) * 0.1f) << ")";
                initVariables = graph_variable.str();
                evaluator.evaluateFloat(initVariables.c_str(), initVariables.length());
                graphY[i] = evaluator.evaluateFloat(buffer, INPUT_OUTPUT_BUFFER_SIZE);
                graphX[i] = ((i - 50) * 0.1f);
            }
            graph_error.assign("");
        }
        catch (std::exception const& e) {
            graph_error.assign(e.what());
        }
    }
}
int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#ifdef __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(800, 600, "ImCalculator", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
    bool err = gladLoadGL(glfwGetProcAddress) == 0; // glad2 recommend using the windowing library loader instead of the (optionally) bundled one.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
    bool err = false;
    glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
    bool err = false;
    glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    ImVec4 font_color = ImVec4(1, 1, 1, 1);
    ImVec4 button_color = ImVec4(0.3f, 0.3f, 0.5f, 1);
    ImVec4 other_color = ImVec4(1, 1, 1, 1);
    ImVec4 titlebar_color = ImVec4(0, 0, 0.2f, 1);
    ImVec4 clear_color_background = ImVec4(0, 0, 0, 1);
    //load seetings conf;
    std::ifstream conf_file("ImCalc.ini");
    if (conf_file) {
        conf_file >> font_color;
        conf_file >> button_color;
        conf_file >> other_color;
        conf_file >> titlebar_color;
        conf_file >> clear_color_background;
    }
    conf_file.close();
    // Our state
    size_t button_count = sizeof(BUTTONS) / sizeof(char*);
    buffer = new char[INPUT_OUTPUT_BUFFER_SIZE];
    bool color_options_window = false;
    bool graph_window = false;
    ImGuiInputTextFlags textflags = ImGuiInputTextFlags_::ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_::ImGuiInputTextFlags_Multiline | ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue;
    ImGuiWindowFlags flags = ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoBringToFrontOnFocus;
    std::vector<std::string> graphFunctions;
    std::fill(buffer, buffer + INPUT_OUTPUT_BUFFER_SIZE - 1, 0);
    std::fill(graphX, graphX + GRAPH_SIZE - 1, 0);
    std::fill(graphY, graphY + GRAPH_SIZE - 1, 0);
    std::string command;
    // Main loop
    ImFont* font1 = nullptr;
    int returnbtnIndex = ImGui::GetKeyIndex(ImGuiKey_::ImGuiKey_Enter);
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        int width;
        int height;
        glfwGetWindowSize(window, &width, &height);
        //// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        //if (show_demo_window)
        //    ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Text, font_color);
            ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, button_color);
            ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_WindowBg, clear_color_background);
            ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_FrameBg, button_color);
            ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_CheckMark, other_color);
            ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_TitleBg, titlebar_color);
            ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_TitleBgActive, titlebar_color);
            ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_TitleBgCollapsed, titlebar_color);
            static float f = 0.0f;
            static int counter = 0;
            if (ImGui::BeginMainMenuBar()) {
                if (ImGui::BeginMenu("Windows")) {
                    if (ImGui::MenuItem("Color Options")) {
                        color_options_window = !color_options_window;
                    }
                    if (ImGui::MenuItem("Graph Window")) {
                        graph_window = !graph_window;
                        fillGraph();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }
            ImVec2 vec2(width, height - 20);
            ImGui::SetNextWindowSize(vec2);
            vec2.x = 0;
            vec2.y = 20;
            ImGui::SetNextWindowPos(vec2);
            ImGui::Begin("My Epic Calculator!", NULL, flags);
            vec2.x = width / 4 * 3;
            vec2.y = height / 4;
            font1 = ImGui::GetFont();
            float buttnscale = min(1, width / 1280.0 * 4);
            font1->Scale = buttnscale;
            ImGui::PushFont(font1);
            ImGui::PushTextWrapPos(vec2.x);
            ImGui::InputTextMultiline("", buffer, INPUT_OUTPUT_BUFFER_SIZE, vec2, textflags);
            ImGui::SameLine();
            vec2.x = width / 4;
            if (ImGui::Button("solve", vec2) || ImGui::IsKeyDown(returnbtnIndex)) {
                evaluator.evaluate(buffer, INPUT_OUTPUT_BUFFER_SIZE);
            }
            ImGui::Text(buffer);
            ImGui::PopTextWrapPos();
            ImGui::PopFont();
            command.assign(buffer);
            vec2.x = width/6;
            vec2.y = height/8;
            ImVec2 scaled(vec2);
            scaled.x = vec2.x / 2;
            font1->Scale = buttnscale;
            ImGui::PushFont(font1);
            for (size_t i = 0; i < button_count; i++) {
                if (i % 7 != 0) {
                    ImGui::SameLine();
                }
                if (i % 7 < 4) {
                    if (ImGui::Button(BUTTONS[i], scaled))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                        command += BUTTONS[i];
                } else {
                    if (ImGui::Button(BUTTONS[i], vec2))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                        command += BUTTONS[i];
                }

            }
            ImGui::PopFont();
            font1->Scale = 1;
            ImGui::PushFont(font1);
            if (command.length() < INPUT_OUTPUT_BUFFER_SIZE)
                strcpy(buffer, command.c_str());

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::PopFont();
            ImGui::End();
        }
        if (color_options_window)
        {
            ImGui::Begin("Color Options", &color_options_window, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

            ImGui::Text("Hello! set the background right here");
            ImGui::ColorEdit3("Clear color", (float*)&clear_color_background);
            ImGui::ColorEdit3("Text color", (float*)&font_color);
            ImGui::ColorEdit3("Button color", (float*)&button_color);
            ImGui::ColorEdit3("Titlebar color", (float*)&titlebar_color);
            ImGui::ColorEdit3("Misc color", (float*)&other_color);
            if (ImGui::Button("Close Me"))
                color_options_window = false;
            ImGui::End();
        }
        if (graph_window)
        {
            ImGui::Begin("Graphing Evaluator", &graph_window, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            if (ImGui::Checkbox("F(y)", &f_of_y)) {
                fillGraph();
                f_of_x = false;
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("F(x)", &f_of_x)) {
                fillGraph();
                f_of_y = false;
            }
            ImGui::SameLine();
            if (ImGui::InputText("", buffer, INPUT_OUTPUT_BUFFER_SIZE)) {
                fillGraph();
            }
            if (graph_error.length() > 0) {
                ImGui::TextColored(error_font_color, graph_error.c_str());
            }
            static bool show_lines = true;
            static bool show_fills = true;
            static float fill_ref = 0;
            ImGui::Checkbox("Lines", &show_lines); ImGui::SameLine();
            ImGui::Checkbox("Fills", &show_fills);
            ImGui::DragFloat("Reference", &fill_ref, 1, -100, 500);

            ImPlot::SetNextPlotLimits(-5, 5, -5, 5);
            if (ImPlot::BeginPlot("Stock Prices", "Days", "Price")) {
                if (show_fills) {
                    ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
                    ImPlot::PlotShaded("Fill", graphX, graphY, GRAPH_SIZE, fill_ref);
                    ImPlot::PopStyleVar();
                }
                if (show_lines) {
                    ImPlot::PlotLine(buffer, graphX, graphY, GRAPH_SIZE);
                }
                ImPlot::EndPlot();
            }
            if (ImGui::Button("Close Me"))
                graph_window = false;
            ImGui::End();
        }

        ImGui::PopStyleColor(8);
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color_background.x, clear_color_background.y, clear_color_background.z, clear_color_background.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    std::ofstream conf_file_out("ImCalc.ini");
    conf_file_out << font_color << std::endl;
    conf_file_out << button_color << std::endl;
    conf_file_out << other_color << std::endl;
    conf_file_out << titlebar_color << std::endl;
    conf_file_out << clear_color_background << std::endl;
    conf_file_out.close();
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    delete[] buffer;
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

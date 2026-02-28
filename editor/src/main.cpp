#include <iostream>
#include <fstream>
#include <future>
#include <memory>
#include <glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "ffmpeg.h"
#include "objects.h"


// preview (codec, processar e mostrar proximo frame, cache de timelines) - figma
struct Timeline{
    float gui_playhead = 0.0f;
    float playhead_time = 0.0f;
    int64_t pts;
    bool isPlaying = false;
    VideoReader* selected_video;

    void update(double dt){
        if (this->isPlaying){
            this->playhead_time += .001;
            this->gui_playhead = this->playhead_time;
            printf("PLYAING: dt %f playhead time: %f pts %f\n", dt, this->playhead_time, this->pts);
        } else {
            this->gui_playhead = this->playhead_time;
            printf("NOTP: dt %f playhead time: %f pts %f\n", dt, this->playhead_time, this->pts);

        }
    }

    void key_callback(int key, int action){
        bool wasPlaying = this->isPlaying;
        if (key == GLFW_KEY_RIGHT){
            if (action == GLFW_PRESS){
                this->playhead_time += 3.0;
                printf("key press right\n");
            } else if (action == GLFW_REPEAT){
                this->isPlaying = false;
                this->playhead_time += 1;
            }
            this->pts = (int64_t)(playhead_time / this->selected_video->get_time_base());
            this->selected_video->seek_frame(this->pts);
        }
        if (key == GLFW_KEY_LEFT){
            if (action == GLFW_PRESS){
                this->playhead_time -= 3.0;
                
            } else if (action == GLFW_REPEAT){
                this->isPlaying = false;
                this->playhead_time -= 1;
            }
            this->pts = (int64_t)(this->playhead_time / this->selected_video->get_time_base());
            this->selected_video->seek_frame(this->pts);
        }
        if (key == GLFW_KEY_R){

            this->selected_video->seek_frame(0);
            // this->gui_playhead = 0.0f;
            // this->pts = 0;

            // avcodec_flush_buffers(this->selected_video->state.av_codec_context);
        }
        this->isPlaying = wasPlaying;
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){
            printf("key space\n");
            this->isPlaying = !this->isPlaying;
        }
    }
};
Timeline tl;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    tl.key_callback(key, action);
}

void log(const char* str){
    std::cout << str << "\0";
}
void* operator new(size_t size){
    log("alocou " + size);
    return malloc(size);
}

void operator delete(void* memory, size_t size) noexcept{
    log("liberou " + size);
    free(memory);
}


int main(){
    if (!glfwInit()){
        log("falha inicializando glfw");
        return -1;
    }
    int frame_width = 640, frame_height = 360;
    VideoReader videoReader("teste.mp4");
    

    // unsigned char* data;
    uint8_t* data = new uint8_t[frame_width*frame_height*4];
    // std::unique_ptr<uint8_t> data = std::make_unique<uint8_t>(frame_width*frame_height*4);
    // if (!video_reader_read_frame("teste.mp4", &data, &frame_width, &frame_height)){
    videoReader.read_frame(&data, &tl.pts);
    std::cout << "frame res " << frame_width << " " << frame_height << std::endl;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    GLFWwindow* window = glfwCreateWindow(frame_width, frame_height, "teste", NULL, NULL);
    if (!window){
        log("falha ao criar janela");
        glfwTerminate();
        return -1;
    }   
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    gladLoadGL();
    glViewport(0, 0, frame_width, frame_height);
    
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    ImGui::CreateContext();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
    // ImGui::GetIO().ConfigFlags |= imGuiconfig;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();
    // double lasttime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
        ImGui::Begin("poggers");                          // Create a window called "Hello, world!" and append into it.
        auto drawlist = ImGui::GetWindowDrawList();
        ImVec2 screenpos = ImGui::GetCursorScreenPos();
        ImVec2 cursorpos = ImGui::GetMousePos();
        drawlist->AddCircle({cursorpos.x, cursorpos.y}, 200, IM_COL32(255,0,0,255), 5, 10);
        drawlist->AddBezierQuadratic(cursorpos, screenpos, {200,200}, IM_COL32(255,0,0,255), 5, 5);
        ImGui::SliderFloat("playback", &tl.gui_playhead, 0, 100);
        ImGui::SliderFloat("playhead", &tl.playhead_time, 0, 100);
        ImGui::End();

        videoReader.read_frame(&data, &tl.pts);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame_width, frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        
        glOrtho(0, frame_width, frame_height, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        
        
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex);
        glBegin(GL_QUADS);
        glTexCoord2d(0,0); glVertex2i(0, 0);
        glTexCoord2d(1,0); glVertex2i(frame_width, 0);
        glTexCoord2d(1,1); glVertex2i(frame_width, frame_height);
        glTexCoord2d(0,1); glVertex2i(0, frame_height);
        glEnd();
        
        glDisable(GL_TEXTURE_2D);
        
        ImGui::Render();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(window); // restaura o contexto
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(window);
        
        // glfwWaitEvents();
        glfwPollEvents();
        
    }
    
    delete[] data;
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}
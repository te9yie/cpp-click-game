#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

namespace {

struct DestroyWindow {
  void operator()(SDL_Window* w) const { SDL_DestroyWindow(w); }
};
using WindowPtr = std::unique_ptr<SDL_Window, DestroyWindow>;

struct DeleteContext {
  void operator()(SDL_GLContext c) const { SDL_GL_DeleteContext(c); }
};
using ContextPtr =
    std::unique_ptr<std::remove_pointer_t<SDL_GLContext>, DeleteContext>;

// GLHandle.
template <class Deleter>
class GLHandle {
 private:
  GLHandle(const GLHandle&) = delete;
  GLHandle& operator=(const GLHandle&) = delete;

 private:
  GLuint id_ = 0;

 public:
  GLHandle() = default;
  GLHandle(GLuint id) : id_(id) {}
  GLHandle(GLHandle&& handle) : id_(handle.id_) { handle.id_ = 0; }
  virtual ~GLHandle() { Deleter()(id_); }

  GLuint get() const { return id_; }
  explicit operator bool() const { return id_ != 0; }

  GLHandle& operator=(GLHandle&& handle) {
    GLHandle(std::move(handle)).swap(*this);
    return *this;
  }

  void swap(GLHandle& handle) {
    using std::swap;
    swap(id_, handle.id_);
  }
};

template <class Deleter>
inline void swap(GLHandle<Deleter>& lhs, GLHandle<Deleter>& rhs) {
  lhs.swap(rhs);
}

struct ShaderDelete {
  void operator()(GLuint id) const { glDeleteShader(id); }
};
using ShaderHandle = GLHandle<ShaderDelete>;

struct ProgramDelete {
  void operator()(GLuint id) const { glDeleteProgram(id); }
};
using ProgramHandle = GLHandle<ProgramDelete>;

struct BufferDelete {
  void operator()(GLuint id) const { glDeleteBuffers(1, &id); }
};
using BufferHandle = GLHandle<BufferDelete>;

struct VertexArrayDelete {
  void operator()(GLuint id) const { glDeleteVertexArrays(1, &id); }
};
using VertexArrayHandle = GLHandle<VertexArrayDelete>;

ShaderHandle compile_source(GLenum type, const char* version, const char* src) {
  auto id = glCreateShader(type);
  if (id == 0) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "compile_source: %s",
                 gluErrorString(glGetError()));
    return 0;
  }

  const GLchar* srcs[] = {version, src};
  glShaderSource(id, 2, srcs, nullptr);
  glCompileShader(id);

  GLint status;
  glGetShaderiv(id, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    GLint len;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
      const GLsizei BUF_SIZE = 1024;
      char buf[BUF_SIZE];
      GLsizei size;
      glGetShaderInfoLog(id, BUF_SIZE, &size, buf);
      SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "compile_source: %s", buf);
    }
    return 0;
  }
  return id;
}

ProgramHandle create_program(const char* version, const char* vert_src,
                             const char* frag_src) {
  auto id = glCreateProgram();
  if (id == 0) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "create_program: %s",
                 gluErrorString(glGetError()));
    return 0;
  }
  auto vert_shader = compile_source(GL_VERTEX_SHADER, version, vert_src);
  if (!vert_shader) {
    return 0;
  }
  auto frag_shader = compile_source(GL_FRAGMENT_SHADER, version, frag_src);
  if (!frag_shader) {
    return 0;
  }
  glAttachShader(id, vert_shader.get());
  glAttachShader(id, frag_shader.get());
  glLinkProgram(id);

  GLint status;
  glGetProgramiv(id, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    GLint len;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
      const GLsizei BUF_SIZE = 1024;
      char buf[BUF_SIZE];
      GLsizei size;
      glGetProgramInfoLog(id, BUF_SIZE, &size, buf);
      SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "create_program: %s", buf);
    }
    return 0;
  }
  return id;
}

BufferHandle create_buffer() {
  GLuint id;
  glGenBuffers(1, &id);
  return id;
}

VertexArrayHandle create_vertex_array() {
  GLuint id;
  glGenVertexArrays(1, &id);
  return id;
}

const char* SHADER_VERSION = "#version 400\n";
const char* VERT_SRC =
    "in vec3 inPosition;"
    "in vec4 inColor;"
    "out vec4 Color;"
    "void main() {"
    "  Color = inColor;"
    "  gl_Position = vec4(inPosition, 1.0);"
    "}";
const char* FRAG_SRC =
    "in vec4 Color;"
    "out vec4 FragColor;"
    "void main() {"
    "  FragColor = Color;"
    "}";

struct VertexFormat {
  float pos[3];
  float color[3];
};

}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
#if defined(_MSC_VER)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  const char* TITLE = "Click Game";
  const int SCREEN_WIDTH = 4 * 200;
  const int SCREEN_HEIGHT = 3 * 200;

  SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

  if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0) {
    SDL_LogCritical(SDL_LOG_CATEGORY_SYSTEM, "%s", SDL_GetError());
    return EXIT_FAILURE;
  }
  atexit(SDL_Quit);

  WindowPtr window(SDL_CreateWindow(
      TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
      SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL));
  if (!window) {
    SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "%s", SDL_GetError());
    return EXIT_FAILURE;
  }

  ContextPtr context(SDL_GL_CreateContext(window.get()));
  if (!context) {
    SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "%s", SDL_GetError());
    return EXIT_FAILURE;
  }
  SDL_GL_MakeCurrent(window.get(), context.get());
  SDL_GL_SetSwapInterval(1);

  if (auto r = glewInit(); r != GLEW_OK) {
    SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "%s", glewGetErrorString(r));
    return EXIT_FAILURE;
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGui_ImplSDL2_InitForOpenGL(window.get(), context.get());
  ImGui_ImplOpenGL3_Init("#version 130");

  {
    int major;
    int minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    SDL_Log("GL Vendor: %s", glGetString(GL_VENDOR));
    SDL_Log("GL Renderer: %s", glGetString(GL_RENDERER));
    SDL_Log("GL Version: %s (%d.%d)", glGetString(GL_VERSION), major, minor);
    SDL_Log("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
  }

  const VertexFormat vertices[] = {
      // clang-format off
      { { 0, 0.5f, 0 }, { 1, 0, 0 } },
      { { 0.5f, -0.5f, 0 }, { 0, 1, 0 } },
      { { -0.5f, -0.5f, 0 }, { 0, 0, 1 } },
      // clang-format on
  };

  auto shader = create_program(SHADER_VERSION, VERT_SRC, FRAG_SRC);
  if (!shader) return EXIT_FAILURE;

  auto vertex_buffer = create_buffer();
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer.get());
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  auto vertex_array = create_vertex_array();
  glBindVertexArray(vertex_array.get());

  auto pos_loc = glGetAttribLocation(shader.get(), "inPosition");
  auto col_loc = glGetAttribLocation(shader.get(), "inColor");

  glEnableVertexAttribArray(pos_loc);
  glEnableVertexAttribArray(col_loc);

  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer.get());
  glVertexAttribPointer(
      pos_loc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat),
      reinterpret_cast<const void*>(offsetof(VertexFormat, pos)));
  glVertexAttribPointer(
      col_loc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat),
      reinterpret_cast<const void*>(offsetof(VertexFormat, color)));

  bool loop = true;
  while (loop) {
    {
      SDL_Event e;
      while (SDL_PollEvent(&e)) {
        ImGui_ImplSDL2_ProcessEvent(&e);
        if (e.type == SDL_QUIT) {
          loop = false;
        }
        if (e.type == SDL_WINDOWEVENT &&
            e.window.event == SDL_WINDOWEVENT_CLOSE &&
            e.window.windowID == SDL_GetWindowID(window.get())) {
          loop = false;
        }
      }
    }

    int w, h;
    SDL_GL_GetDrawableSize(window.get(), &w, &h);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();

    ImGui::Render();

    glViewport(0, 0, w, h);
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader.get());
    glBindVertexArray(vertex_array.get());
    glDrawArrays(GL_TRIANGLES, 0, 3);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glFinish();
    SDL_GL_SwapWindow(window.get());
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  return 0;
}
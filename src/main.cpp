#include <array>
#include <vector>
#include <cstdint>
#include <format>

#include <raylib.h>
#include <raymath.h>

#include <imgui.h>
#include <imgui_impl_raylib.h>

namespace
{

struct Options
{
  int32_t width = 1920;
  int32_t height = 1080;
};

} // namespace anon

namespace render
{

struct Light {
  enum Type : int32_t
  {
    Directional,
    Point
  } type{Point};

  Vector3 position{Vector3Zeros};
  Vector3 target{Vector3Zeros};
  Color color{WHITE};

  bool enabled{true};

  struct Bindings
  {
    int32_t enabledBinding{};
    int32_t typeBinding{};
    int32_t positionBinding{};
    int32_t targetBinding{};
    int32_t colorBinding{};
  } bindings{};
};

void CreateLight(Shader shader, Light& light)
{
  static uint32_t lightIndex = 0;

  light.bindings.enabledBinding = GetShaderLocation(
    shader, std::format("lights[{}].enabled", lightIndex).c_str());
  light.bindings.typeBinding = GetShaderLocation(
    shader, std::format("lights[{}].type", lightIndex).c_str());

  light.bindings.positionBinding = GetShaderLocation(
    shader, std::format("lights[{}].position", lightIndex).c_str());
  light.bindings.targetBinding = GetShaderLocation(
    shader, std::format("lights[{}].target", lightIndex).c_str());
  light.bindings.colorBinding = GetShaderLocation(
    shader, std::format("lights[{}].color", lightIndex).c_str());

  ++lightIndex;
}

void UpdateLight(Shader shader, const Light& light)
{
  const int32_t lightEnabled = light.enabled;
  SetShaderValue(
    shader, light.bindings.enabledBinding, &lightEnabled, SHADER_UNIFORM_INT);
  const int32_t lightType = light.type;
  SetShaderValue(
    shader, light.bindings.typeBinding, &lightType, SHADER_UNIFORM_INT);

  const std::array lightPosition{
    light.position.x,
    light.position.y,
    light.position.z};
  SetShaderValue(
    shader, light.bindings.positionBinding, lightPosition.data(), SHADER_UNIFORM_VEC3);

  const std::array targetPosition{
    light.target.x,
    light.target.y,
    light.target.z};
  SetShaderValue(
    shader, light.bindings.targetBinding, targetPosition.data(), SHADER_UNIFORM_VEC3);

  const std::array color {
    static_cast<float>(light.color.r) / 255.0f,
    static_cast<float>(light.color.g) / 255.0f,
    static_cast<float>(light.color.b) / 255.0f,
    static_cast<float>(light.color.a) / 255.0f};
  SetShaderValue(
    shader, light.bindings.colorBinding, color.data(), SHADER_UNIFORM_VEC4);
}

} // namespace render

int main() {
  Options options;

  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(options.width, options.height, "CanSat Visual");

  const Vector3 position = {0.0f, 3.0f, 0.0f};

  Camera camera {
    .position = {15.0f, 6.0f, 0.0f},
    .target = position,
    .up = {0.0f, 1.0f, 0.0f},
    .fovy = 45.0f,
    .projection = CAMERA_PERSPECTIVE
  };

  Shader shader = LoadShader(
    "shaders/lighting.vs",
    "shaders/lighting.fs");
  shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

  const std::array ambient{0.3f, 0.3f, 0.3f, 1.f};
  SetShaderValue(shader, GetShaderLocation(shader, "ambient"), &ambient, SHADER_UNIFORM_VEC4);

  SetTargetFPS(60);

  auto model = LoadModel("cansat.glb");
  model.materials[0].shader = shader;
  model.materials[1].shader = shader;

  std::vector<render::Light> lights;
  render::CreateLight(shader, lights.emplace_back(render::Light{
    .type = render::Light::Point,
    .position = camera.position,
    .color = RED,
    .enabled = true
  }));

  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void) io;
  ImGui::StyleColorsDark();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  ImGui_ImplRaylib_Init();

  while (!WindowShouldClose())
  {
    ImGui_ImplRaylib_ProcessEvents();

    const std::array cameraPosition{
      position.x,
      position.y,
      position.z};
    SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPosition.data(), SHADER_UNIFORM_VEC3);

    for (const auto& light : lights)
    {
      render::UpdateLight(shader, light);
    }

    // ImGui render frame
    {
      ImGui_ImplRaylib_NewFrame();
      ImGui::NewFrame();

      static bool isDebugOpen = false;
      if (IsKeyPressed(KEY_GRAVE)) {
        isDebugOpen = !isDebugOpen;
      }

      ImGui::Begin("Debug", &isDebugOpen);
      ImGui::Text("piccee");
      ImGui::End();

      ImGui::EndFrame();
      ImGui::Render();
    }

    {
      BeginDrawing();
      ClearBackground({33, 33, 33, 255});

      // Render
      {
        BeginMode3D(camera);
        {
          BeginShaderMode(shader);
          {
            DrawModel(model, position, 1.0f, WHITE);
          }
          EndShaderMode();
        }
        EndMode3D();
      }

      // Interface
      {
        ImGui_ImplRaylib_RenderDrawData(ImGui::GetDrawData());
      }

      EndDrawing();
    }
  }

  ImGui_ImplRaylib_Shutdown();
  ImGui::DestroyContext();

  return 0;
}

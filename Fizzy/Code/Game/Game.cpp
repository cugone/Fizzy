#include "Game/Game.hpp"

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/DataUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/KerningFont.hpp"

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector4.hpp"

#include "Engine/Renderer/AnimatedSprite.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Window.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GameConfig.hpp"

void DebugVisualizeSAT(const OBB2& _test2_OBB2, const OBB2& _test1_OBB2);

void Game::Initialize() {
    g_theRenderer->RegisterMaterialsFromFolder(std::string{ "Data/Materials" });
    _test1_OBB2.AddPaddingToSides(25.0f, 25.0f);
    _test1_OBB2.Translate(Vector2(200.0f, 250.0f));
    _test2_OBB2.AddPaddingToSides(25.0f, 25.0f);
    _test2_OBB2.Translate(Vector2(300.0f, 250.0f));
    _test1_OBB2.SetOrientationDegrees(0.0f);
    _test2_OBB2.SetOrientationDegrees(0.0f);
}

void Game::BeginFrame() {
    /* DO NOTHING */
}

void Game::Update(TimeUtils::FPSeconds deltaSeconds) {
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::Esc)) {
        g_theApp->SetIsQuitting(true);
        return;
    }

    Camera2D& base_camera = _ui_camera;
    HandleDebugInput(base_camera);
    HandlePlayerInput(base_camera);
    base_camera.Update(deltaSeconds);

    if(g_theInputSystem->IsKeyDown(KeyCode::R)) {
        _test2_OBB2.SetOrientationDegrees(0.0f);
        _test1_OBB2.SetOrientationDegrees(0.0f);
    }

    float speed = 50.0f;
    if(g_theInputSystem->IsKeyDown(KeyCode::D)) {
        _test2_OBB2.Translate(Vector2::X_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::A)) {
        _test2_OBB2.Translate(-Vector2::X_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::W)) {
        _test2_OBB2.Translate(-Vector2::Y_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::S)) {
        _test2_OBB2.Translate(Vector2::Y_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::NumPad6)) {
        _test1_OBB2.Translate(Vector2::X_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::NumPad4)) {
        _test1_OBB2.Translate(-Vector2::X_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::NumPad8)) {
        _test1_OBB2.Translate(-Vector2::Y_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::NumPad5)) {
        _test1_OBB2.Translate(Vector2::Y_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::Q)) {
        _test2_OBB2.SetOrientationDegrees(_test2_OBB2.orientationDegrees - 45.0f * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::E)) {
        _test2_OBB2.SetOrientationDegrees(_test2_OBB2.orientationDegrees + 45.0f * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::NumPad7)) {
        _test1_OBB2.SetOrientationDegrees(_test1_OBB2.orientationDegrees - 45.0f * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::NumPad9)) {
        _test1_OBB2.SetOrientationDegrees(_test1_OBB2.orientationDegrees + 45.0f * deltaSeconds.count());
    }

    //_test1_OBB2.RotateDegrees(45.0f * deltaSeconds.count());
    //_test_AABB2.RotateDegrees(-45.0f * deltaSeconds.count());
    //auto pos = g_theInputSystem->GetCursorWindowPosition(*g_theRenderer->GetOutput()->GetWindow());
    //_test_AABB2.position = pos;
    _do_overlap = MathUtils::DoOBBsOverlap(_test2_OBB2, _test1_OBB2);
}


void DebugVisualizeSAT(const OBB2& _test2_OBB2, const OBB2& _test1_OBB2) {
    const auto aR = Matrix4::Create2DRotationDegreesMatrix(_test2_OBB2.orientationDegrees);
    const auto aT = Matrix4::CreateTranslationMatrix(_test2_OBB2.position);
    const auto aM = aT * aR;
    const auto a_p = _test2_OBB2.position;
    const auto a_hex = _test2_OBB2.half_extents.x;
    const auto a_hey = _test2_OBB2.half_extents.y;
    const auto a_topright = aM.TransformPosition(Vector2(+a_hex, -a_hey));
    const auto a_bottomright = aM.TransformPosition(Vector2(+a_hex, +a_hey));
    const auto a_topleft = aM.TransformPosition(Vector2(-a_hex, -a_hey));
    const auto a_bottomleft = aM.TransformPosition(Vector2(-a_hex, +a_hey));
    const auto a_right_normal = aR.TransformDirection(Vector2(a_hex, 0.0f).GetNormalize());
    const auto a_down_normal = aR.TransformDirection(Vector2(0.0f, a_hey).GetNormalize());
    const auto a_left_normal = -a_right_normal;
    const auto a_up_normal = -a_down_normal;
    const std::vector<Vertex3D> a_vbo_lines{ {Vector3{a_bottomleft}}, {Vector3{a_topleft}}, {Vector3{a_topright}}, {Vector3{a_bottomright}} };
    const std::vector<unsigned int> a_ibo_lines{ 0, 1, 2, 3, 0 };
    const std::vector<Vertex3D> a_vbo_normals{ {Vector3{a_p} +Vector3{a_right_normal * a_hex}}, {Vector3{a_p} +Vector3{a_right_normal * (a_hex + 5.0f)}}, {Vector3{a_p} +Vector3{a_down_normal * a_hey}}, {Vector3{a_p} +Vector3{a_down_normal * (a_hex + 5.0f)}}, {Vector3{a_p} +Vector3{a_left_normal * (a_hex)}}, {Vector3{a_p} +Vector3{a_left_normal * (a_hex + 5.0f)}}, {Vector3{a_p} +Vector3{a_up_normal * (a_hex)}}, {Vector3{a_p} +Vector3{a_up_normal * (a_hex + 5.0f)}} };
    const std::vector<unsigned int> a_ibo_normals{ 0, 1, 2, 3, 4, 5, 6, 7 };
    g_theRenderer->DrawIndexed(PrimitiveType::Points, a_vbo_lines, a_ibo_lines);
    g_theRenderer->DrawIndexed(PrimitiveType::Lines, a_vbo_normals, a_ibo_normals);

    const auto bR = Matrix4::Create2DRotationDegreesMatrix(_test1_OBB2.orientationDegrees);
    const auto bT = Matrix4::CreateTranslationMatrix(_test1_OBB2.position);
    const auto bM = bT * bR;
    const auto b_p = _test1_OBB2.position;
    const auto b_hex = _test1_OBB2.half_extents.x;
    const auto b_hey = _test1_OBB2.half_extents.y;
    const auto b_topright = bM.TransformPosition(Vector2(+b_hex, -b_hey));
    const auto b_bottomright = bM.TransformPosition(Vector2(+b_hex, +b_hey));
    const auto b_topleft = bM.TransformPosition(Vector2(-b_hex, -b_hey));
    const auto b_bottomleft = bM.TransformPosition(Vector2(-b_hex, +b_hey));
    const auto b_right_normal = bR.TransformDirection(Vector2(b_hex, 0.0f).GetNormalize());
    const auto b_down_normal = bR.TransformDirection(Vector2(0.0f, b_hey).GetNormalize());
    const auto b_left_normal = -b_right_normal;
    const auto b_up_normal = -b_down_normal;

    const std::vector<Vertex3D> b_vbo_lines{ {Vector3{b_bottomleft}}, {Vector3{b_topleft}}, {Vector3{b_topright}}, {Vector3{b_bottomright}} };
    const std::vector<unsigned int> b_ibo_lines{ 0, 1, 2, 3, 0 };
    const std::vector<Vertex3D> b_vbo_normals{ {Vector3{b_p} +Vector3{b_right_normal * b_hex}}, {Vector3{b_p} +Vector3{b_right_normal * (b_hex + 5.0f)}}, {Vector3{b_p} +Vector3{b_down_normal * b_hey}}, {Vector3{b_p} +Vector3{b_down_normal * (b_hex + 5.0f)}}, {Vector3{b_p} +Vector3{b_left_normal * (b_hex)}}, {Vector3{b_p} +Vector3{b_left_normal * (b_hex + 5.0f)}}, {Vector3{b_p} +Vector3{b_up_normal * (b_hex)}}, {Vector3{b_p} +Vector3{b_up_normal * (b_hex + 5.0f)}} };
    const std::vector<unsigned int> b_ibo_normals{ 0, 1, 2, 3, 4, 5, 6, 7 };
    g_theRenderer->DrawIndexed(PrimitiveType::Points, b_vbo_lines, b_ibo_lines);
    g_theRenderer->DrawIndexed(PrimitiveType::Lines, b_vbo_normals, b_ibo_normals);

    //const std::vector<Vector2> a_normals{ a_left_normal, a_up_normal, a_right_normal, a_down_normal };
    const std::vector<Vector2> a_normals{ a_right_normal, a_down_normal };
    const std::vector<Vector2> a_corners{ a_bottomleft, a_topleft, a_topright, a_bottomright };
    //const std::vector<Vector2> b_normals{ b_left_normal, b_up_normal, b_right_normal, b_down_normal };
    const std::vector<Vector2> b_normals{ b_right_normal, b_down_normal };
    const std::vector<Vector2> b_corners{ b_bottomleft, b_topleft, b_topright, b_bottomright };

    auto min_aa_x = std::numeric_limits<float>::infinity();
    auto max_aa_x = std::numeric_limits<float>::lowest();
    auto min_ba_x = std::numeric_limits<float>::infinity();
    auto max_ba_x = std::numeric_limits<float>::lowest();
    auto min_ab_x = std::numeric_limits<float>::infinity();
    auto max_ab_x = std::numeric_limits<float>::lowest();
    auto min_bb_x = std::numeric_limits<float>::infinity();
    auto max_bb_x = std::numeric_limits<float>::lowest();
    auto min_aa_y = std::numeric_limits<float>::infinity();
    auto max_aa_y = std::numeric_limits<float>::lowest();
    auto min_ba_y = std::numeric_limits<float>::infinity();
    auto max_ba_y = std::numeric_limits<float>::lowest();
    auto min_ab_y = std::numeric_limits<float>::infinity();
    auto max_ab_y = std::numeric_limits<float>::lowest();
    auto min_bb_y = std::numeric_limits<float>::infinity();
    auto max_bb_y = std::numeric_limits<float>::lowest();

    for(std::size_t i = 0; i < a_normals.size(); ++i) {
        float* min_a = nullptr;
        float* max_a = nullptr;
        const Vector2* an = nullptr;
        float* min_b = nullptr;
        float* max_b = nullptr;
        switch(i) {
        case 0:
            min_a = &min_aa_x;
            max_a = &max_aa_x;
            an = &a_normals[i];
            min_b = &min_ba_x;
            max_b = &max_ba_x;
            break;
        case 1:
            min_a = &min_aa_y;
            max_a = &max_aa_y;
            an = &a_normals[i];
            min_b = &min_ba_y;
            max_b = &max_ba_y;
            break;
        default: /* DO NOTHING */;
        }
        for(const auto& ac : a_corners) {
            const auto proj_dp = MathUtils::DotProduct(ac, *an);
            *min_a = (std::min)(*min_a, proj_dp);
            *max_a = (std::max)(*max_a, proj_dp);
        }
        for(const auto& bc : b_corners) {
            const auto proj_dp = MathUtils::DotProduct(bc, *an);
            *min_b = (std::min)(*min_b, proj_dp);
            *max_b = (std::max)(*max_b, proj_dp);
        }
    }
    for(std::size_t i = 0; i < b_normals.size(); ++i) {
        float* min_a = nullptr;
        float* max_a = nullptr;
        const Vector2* bn = nullptr;
        float* min_b = nullptr;
        float* max_b = nullptr;
        switch(i) {
        case 0:
            min_a = &min_ab_x;
            max_a = &max_ab_x;
            bn = &b_normals[i];
            min_b = &min_bb_x;
            max_b = &max_bb_x;
            break;
        case 1:
            min_a = &min_ab_y;
            max_a = &max_ab_y;
            bn = &b_normals[i];
            min_b = &min_bb_y;
            max_b = &max_bb_y;
            break;
        default: /* DO NOTHING */;
        }
        for(const auto& ac : a_corners) {
            const auto proj_dp = MathUtils::DotProduct(ac, *bn);
            *min_a = (std::min)(*min_a, proj_dp);
            *max_a = (std::max)(*max_a, proj_dp);
        }
        for(const auto& bc : b_corners) {
            const auto proj_dp = MathUtils::DotProduct(bc, *bn);
            *min_b = (std::min)(*min_b, proj_dp);
            *max_b = (std::max)(*max_b, proj_dp);
        }
    }
    const std::vector<Vertex3D> x_vbo{ {Vector3::X_AXIS * min_aa_x}, {Vector3::X_AXIS * max_aa_x},{Vector3::X_AXIS * min_bb_x}, {Vector3::X_AXIS * max_bb_x} };
    const std::vector<unsigned int> x_ibo{ 0, 1, 2, 3 };
    const std::vector<Vertex3D> y_vbo{ {Vector3::Y_AXIS * min_aa_y}, {Vector3::Y_AXIS * max_aa_y},{Vector3::Y_AXIS * min_bb_y}, {Vector3::Y_AXIS * max_bb_y} };
    const std::vector<unsigned int> y_ibo{ 0, 1, 2, 3 };

    g_theRenderer->DrawIndexed(PrimitiveType::Lines, x_vbo, x_ibo);
    g_theRenderer->DrawIndexed(PrimitiveType::Lines, y_vbo, y_ibo);
}

void Game::Render() const {
    g_theRenderer->ResetModelViewProjection();
    g_theRenderer->SetRenderTargetsToBackBuffer();
    g_theRenderer->ClearDepthStencilBuffer();

    g_theRenderer->ClearColor(Rgba::Black);

    g_theRenderer->SetViewportAsPercent();

    //2D View / HUD
    const float ui_view_height = GRAPHICS_OPTION_WINDOW_HEIGHT;
    const float ui_view_width = ui_view_height * _ui_camera.GetAspectRatio();
    const auto ui_view_extents = Vector2{ ui_view_width, ui_view_height };
    const auto ui_view_half_extents = ui_view_extents * 0.5f;
    auto ui_leftBottom = Vector2{ -ui_view_half_extents.x, ui_view_half_extents.y };
    auto ui_rightTop = Vector2{ ui_view_half_extents.x, -ui_view_half_extents.y };
    auto ui_nearFar = Vector2{ 0.0f, 1.0f };
    //auto ui_cam_pos = ui_view_half_extents;
    //_ui_camera.position = ui_cam_pos;
    //_ui_camera.orientation_degrees = 0.0f;
    _ui_camera.SetupView(ui_leftBottom, ui_rightTop, ui_nearFar, MathUtils::M_16_BY_9_RATIO);
    g_theRenderer->SetCamera(_ui_camera);

    g_theRenderer->DrawAxes(static_cast<float>(std::max(ui_view_extents.x, ui_view_extents.y)));

    g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("__2D"));
    //auto S = Matrix4::CreateScaleMatrix(_test1_OBB2.half_extents);
    //auto R = Matrix4::Create2DRotationDegreesMatrix(_test1_OBB2.orientationDegrees);
    //auto T = Matrix4::CreateTranslationMatrix(_test1_OBB2.position);
    //auto S = Matrix4::I;
    //auto R = Matrix4::I;
    //auto T = Matrix4::I;
    //auto M = T * R * S;
    //g_theRenderer->SetModelMatrix(M);
    g_theRenderer->DrawOBB2(_test1_OBB2, _do_overlap ? Rgba::White : Rgba::Red, Rgba::NoAlpha);
    //g_theRenderer->DrawAABB2(_test1_OBB2, _do_overlap ? Rgba::White : Rgba::Red, Rgba::NoAlpha);

    //S = Matrix4::CreateScaleMatrix(_test2_OBB2.half_extents);
    //R = Matrix4::Create2DRotationDegreesMatrix(_test2_OBB2.orientationDegrees);
    //T = Matrix4::CreateTranslationMatrix(_test2_OBB2.position);
    //S = Matrix4::CreateScaleMatrix(_test2_OBB2.CalcDimensions() * 0.5f);
    //R = Matrix4::I;
    //T = Matrix4::CreateTranslationMatrix(_test2_OBB2.CalcCenter());
    //M = T * R * S;
    //g_theRenderer->SetModelMatrix(M);
    //g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("__2D"));
    g_theRenderer->DrawOBB2(_test2_OBB2, _do_overlap ? Rgba::White : Rgba::Green, Rgba::NoAlpha);
    //g_theRenderer->DrawAABB2(_test2_OBB2, _do_overlap ? Rgba::White : Rgba::Green, Rgba::NoAlpha);

    //TEST SAT code Visually--------------------------------------------
    DebugVisualizeSAT(_test2_OBB2, _test1_OBB2);
    //TEST SAT code Visually--------------------------------------------

    auto T = Matrix4::CreateTranslationMatrix(Vector2(2.0f, g_theRenderer->GetFont("System32")->GetLineHeight()));
    auto R = Matrix4::I;
    auto S = Matrix4::I;
    auto M = T * R * S;
    g_theRenderer->SetModelMatrix(M);
    g_theRenderer->DrawTextLine(g_theRenderer->GetFont("System32"), _do_overlap ? "Yes" : "No");

}

void Game::EndFrame() {
    /* DO NOTHING */
}

void Game::HandlePlayerInput(Camera2D& /*base_camera*/) {
    /* DO NOTHING */
}

void Game::HandleDebugInput(Camera2D& base_camera) {
    HandleDebugKeyboardInput(base_camera);
    HandleDebugMouseInput(base_camera);
}

void Game::HandleDebugKeyboardInput(Camera2D& base_camera) {
    if(g_theUISystem->GetIO().WantCaptureKeyboard) {
        return;
    }
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::F1)) {
        _show_debug_window = !_show_debug_window;
    }
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::F4)) {
        g_theUISystem->ToggleImguiDemoWindow();
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::I)) {
        base_camera.Translate(-Vector2::Y_AXIS * 10.0f);
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::J)) {
        base_camera.Translate(-Vector2::X_AXIS * 10.0f);
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::K)) {
        base_camera.Translate(Vector2::Y_AXIS * 10.0f);
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::L)) {
        base_camera.Translate(Vector2::X_AXIS * 10.0f);
    }
}

void Game::HandleDebugMouseInput(Camera2D& /*base_camera*/) {
    if(g_theUISystem->GetIO().WantCaptureMouse) {
        return;
    }
}

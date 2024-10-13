#include <algorithm>
#include <format>
#include <print>
#include <stdint.h>
#include <string>

// credits: wav
float normalize(float angle)
{
    auto revolutions = angle / 360.f;
    if (angle > 180.f || angle < -180.f)
    {
        revolutions = std::round(std::abs(revolutions));
        if (angle < 0.f)
            angle = (angle + 360.f * revolutions);
        else
            angle = (angle - 360.f * revolutions);
        return angle;
    }
    return angle;
}

/**
 * @brief computes the angle as the server and hit registration would.
 * @param viewangle
 * @return
 */
float compute_body_pitch(float viewangle)
{
    // reference:
    // CBasePlayerAnimState::ComputePoseParam_BodyPitch
    // https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/game/shared/base_playeranimstate.cpp#L719-L724

    if (viewangle > 180.0f)
        viewangle -= 360.0f;

    return std::clamp(viewangle, -90.0f, 90.0f);
}

/**
 * @brief determines whether or not there's a desync between networked angle and
 * server's body pitch.
 * @param viewangle the player's rotation angle.
 * @param goal_fake
 * @return
 */
bool is_fake_pitch(const float& viewangle, const float& goal_fake)
{
    const float eye_angles = std::abs(normalize(viewangle));

    if (eye_angles == goal_fake && compute_body_pitch(viewangle) != eye_angles)
        return true;

    return false;
}

/**
 * @brief find viewangle that would produce a fake angle
 * @param goal 'real' angle
 * @return required pitch angle to achieve goal 'real' angle
 */
float find_fake_pitch(const float& goal)
{
    // usage:
    // cmd->viewangles.x = find_fake_pitch(0.0f );
    // cmd->viewangles.x = find_fake_pitch(180.0f);

    for (float angle = 270.0f; angle < 1000.0f; angle += 1.0f)
    {
        if (is_fake_pitch(angle, std::abs(goal)))
        {
            if (goal < 0.0f)
                return -angle;
            else
                return angle;
        }
    }

    return std::numeric_limits<float>::max();
}

int main()
{
    constexpr std::string_view fmt = "Angle: {}. 'Real' angle: {}\n";

    for (float goal_x = -90.0f; goal_x <= 180.0f; goal_x += 90.0f)
    {
        std::print(fmt.data(), find_fake_pitch(goal_x), goal_x);
    }

    return 0;
}

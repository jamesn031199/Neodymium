#version 460 core

layout(location = 0) in  vec4 i_c;
layout(location = 0) out vec4 o_c;

void main()
{
    vec2  p = gl_PointCoord * vec2(2.0) - vec2(1.0);
    float l = clamp(1.0 - length(p), 0.0, 1.0);
    o_c = vec4(i_c.rgb, i_c.a * l);
}

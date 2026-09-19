#version 460 core

layout(location = 0) in  vec2 o_uv;
layout(location = 0) out vec4 o_c;

layout(set = 0, binding = 0) uniform sampler2D u_image;

void main()
{
    o_c = texture(u_image, o_uv);
}

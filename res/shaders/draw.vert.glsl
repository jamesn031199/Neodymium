#version 460 core

layout(location = 0) in vec2  i_p;
layout(location = 1) in float i_z;
layout(location = 2) in uint  i_s;
layout(location = 3) in vec4  i_c;

void main()
{
    gl_Position = vec4(i_p, i_z, 1);
}

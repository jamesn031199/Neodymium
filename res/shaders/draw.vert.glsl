#version 460 core

layout(location = 0) in  vec2  i_p;
layout(location = 1) in  float i_z;
layout(location = 2) in  uint  i_s;
layout(location = 3) in  vec4  i_c;
layout(location = 0) out vec4  o_c;

layout(constant_id = 0) const float point_size_min  = 0.0;
layout(constant_id = 1) const float point_size_max  = 0.0;
layout(constant_id = 2) const float point_size_step = 0.0;
layout(constant_id = 3) const float aspect_ratio    = 1.0;

void main()
{
    o_c          = i_c;
    gl_Position  = vec4(i_p.x / aspect_ratio, i_p.y, i_z, 1);
    gl_PointSize = clamp(float(i_s) * point_size_step, point_size_min, point_size_max);
}

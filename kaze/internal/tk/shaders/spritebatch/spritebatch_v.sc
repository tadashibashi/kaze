$input a_position, a_texcoord0, a_color0
$output v_texcoord0, v_color0

#include <kaze_shader.sc>

void main()
{
    gl_Position = mul(u_viewProj, vec4(a_position, 1.0));
    v_color0 = a_color0;
    v_texcoord0 = a_texcoord0;
}

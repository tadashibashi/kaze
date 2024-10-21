$input a_position, a_texcoord0
$output v_texcoord0

#include <kaze_shader.sc>

void main()
{
    gl_Position = mul(u_viewProj, vec4(a_position.xy, 0.0, 1.0));
    v_texcoord0 = a_texcoord0;
}

$input v_texcoord0, v_color0

#include <kaze_shader.sc>

void main()
{
    gl_FragColor = vec4(texture2D(s_texture0, v_texcoord0)) * v_color0;
}

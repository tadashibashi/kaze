$ v_color0, v_texcoord0

#include <kaze_shader.sc>

void main()
{
    vec4 texel = texture2D(s_texture0, v_texcoord0);
    gl_FragColor = texel * v_color0;
}

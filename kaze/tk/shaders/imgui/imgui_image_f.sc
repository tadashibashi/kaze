$input v_texcoord0

#include <kaze_shader.sc>

uniform vec4 u_imageLodEnabled;

#define u_imageLod     u_imageLodEnabled.x
#define u_imageEnabled u_imageLodEnabled.y

void main()
{
    vec3 color = texture2DLod(s_texture0, v_texcoord0, u_imageLod).xyz;
    float alpha = 0.2 + 0.8 * u_imageEnabled;
    gl_FragColor = vec4(color, alpha);
}

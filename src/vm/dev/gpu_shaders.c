char *gpu_fb_fshader = "#version 330\n#pragma optimize(on)\nout vec4 c;in vec2 t;uniform sampler2D d;void main(){vec4 tc=texture(d,t);c=vec4(tc.rgb,0.5);}";
char *gpu_fb_vshader = "#version 330\n#pragma optimize(on)\nlayout (location = 0) in vec3 p;layout (location = 1) in vec2 a;out vec2 t;uniform mat4 m;void main(){t=a;gl_Position=vec4(vec3(m*vec4(p,1.0)),1.0);}";


#version 150

uniform sampler2D uTex0;

in vec4		Color;
in vec3		Normal;
in vec2		TexCoord;

out vec4 	oColor;

#define TWO_PI 6.28318530718


// START HSV

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

// All components are in the range [0…1], including hue.
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

// END HSV



void main( void )
{
    
    float d = distance(TexCoord,vec2(0.5));
    
    vec3 color;
    
    // snippet from https://thebookofshaders.com/06/
    
    // Use polar coordinates instead of cartesian
    vec2 toCenter = vec2(0.5)-TexCoord;
    float angle = atan(toCenter.y,toCenter.x);
    float radius = length(toCenter)*2.0;

    // Map the angle (-PI to PI) to the Hue (from 0 to 1)
    // and the Saturation to the radius
    color = hsv2rgb(vec3((angle/TWO_PI)+0.5,radius,1.0));

    oColor = vec4(color.x,color.y,color.z,1.0);
    
    
}

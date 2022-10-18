#version 120

uniform vec3      iResolution;           // viewport resolution (in pixels)
uniform float     iTime;                 // shader playback time (in seconds)
uniform vec4      iMouse;                // mouse pixel coords. xy: current (if MLB down), zw: click
varying vec4      globalColor;

void main(){
    vec2 p = (gl_FragCoord.xy / iResolution.xy);

    gl_FragColor = globalColor;
//    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}

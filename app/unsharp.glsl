uniform lowp sampler2D src;
uniform lowp float qt_Opacity; 
varying highp vec2 qt_TexCoord0;

uniform float imgWidth;
uniform float imgHeight;
uniform float coeff_blur;

#define effect_width 1.0

#define Src(a,b) texture2D(src, qt_TexCoord0 + vec2(a,b))

vec4 hook()
{
        float dx = effect_width/imgWidth;
        float dy = effect_width/imgHeight;

	// Retrieves the original pixel
	vec4 orig = Src(0,0);

	// Calculates blurred image (gaussian blur)
	vec4 c1 = Src(-dx,-dy);
	vec4 c2 = Src(0,-dy);
	vec4 c3 = Src(dx,-dy);
	vec4 c4 = Src(-dx,0);
	vec4 c5 = Src(dx,0);
	vec4 c6 = Src(-dx,dy);
	vec4 c7 = Src(0,dy);
	vec4 c8 = Src(dx,dy);

	// gaussian blur filter
	// [ 1, 2 , 1 ]

	// [ 2, 4 , 2 ]
	// [ 1, 2 , 1 ]
	// c1 c2 c3
	// c4    c5
	// c6 c7 c8
	vec4 blur = (c1 + c3 + c6 + c8 + 2 * (c2 + c4 + c5 + c7) + 4 * orig)/16;

	// The blurred image is substracted from the origginal image
        vec4 corr = mix(blur, orig, 1.0 + coeff_blur);

	return corr;
}

void main() {
   //lowp vec4 p = texture2D(src, qt_TexCoord0);
   //   lowp float g = dot(p.xyz, vec3(0.344, 0.5, 0.156));
   gl_FragColor = hook() * qt_Opacity;
}

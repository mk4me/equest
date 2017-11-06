#extension GL_ARB_texture_rectangle : require

// tekstura
uniform sampler2DRect movie_texture;

// ile wynosi stosunek wysokoœci p³aszczyzny Y w stosunku do rozmiaru tekstury?
const float yScale = 2.0/3.0;
// ile wynosi stosunek wysokoœci p³aszczyzny U(V) w stosunku do wysokoœci tekstury?
const float uvScale = 1.0-yScale;

// rozmiar tekstury (pobieranie instrukcjami texture*Size nie zawsze dzia³a)
uniform vec2 texture_size;

void main(void)
{
	// rozmiar tekstury
	float texWidth = texture_size.x;
	float texHeight = texture_size.y;

	// wyznaczenie koordynatów
	vec2 stY = gl_TexCoord[0].st;
	stY.t = stY.t * yScale;
	vec2 stU = gl_TexCoord[0].st;
	stU.s = 0.5 * stU.s;
	stU.t = texHeight * yScale + uvScale * stU.t;
	vec2 stV = vec2(stU.s + texWidth / 2.0, stU.t);
	
	// odczyt komponentów
	float y = texture2DRect(movie_texture, stY).r;
	float u = texture2DRect(movie_texture, stU).r;
	float v = texture2DRect(movie_texture, stV).r;
	
	// konwersja za http://www.fourcc.org/fccyvrgb.php
	y=1.1643*(y-0.0625);
	u=u-0.5;
	v=v-0.5;
	float r=y+1.5958*v;
	float g=y-0.39173*u-0.81290*v;
	float b=y+2.017*u;
	gl_FragColor = gl_Color * vec4(r,g,b,1);
}
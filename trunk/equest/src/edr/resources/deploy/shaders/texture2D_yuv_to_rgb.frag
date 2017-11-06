// tekstura
uniform sampler2D movie_texture;

// ile wynosi stosunek wysokoœci p³aszczyzny Y w stosunku do rozmiaru tekstury?
const float yScale = 2.0/3.0;
// ile wynosi stosunek wysokoœci p³aszczyzny U(V) w stosunku do wysokoœci tekstury?
const float uvScale = 1.0-yScale;

// sta³e które ujednolicaj¹ kod z odpowiednikem dla textureRect
const float texWidth = 1.0;
const float texHeight = 1.0;

void main(void)
{
	// wyznaczenie koordynatów
	vec2 stY = gl_TexCoord[0].st;
	stY.t = stY.t * yScale;
	vec2 stU = gl_TexCoord[0].st;
	stU.s = 0.5 * stU.s;
	stU.t = texHeight * yScale + uvScale * stU.t;
	vec2 stV = vec2(stU.s + texWidth / 2.0, stU.t);
	
	// odczyt komponentów
	float y = texture2D(movie_texture, stY);
	float u = texture2D(movie_texture, stU);
	float v = texture2D(movie_texture, stV);
	
	// konwersja za http://www.fourcc.org/fccyvrgb.php
	y=1.1643*(y-0.0625);
	u=u-0.5;
	v=v-0.5;
	float r=y+1.5958*v;
	float g=y-0.39173*u-0.81290*v;
	float b=y+2.017*u;
	gl_FragColor = gl_Color * vec4(r,g,b,1);
}
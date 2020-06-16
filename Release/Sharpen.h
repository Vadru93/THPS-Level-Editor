#define USE_ADDITIONAL_SHADER 1
//My redefinitions
#define s0 screenSampler
#define width BUFFER_WIDTH
#define height BUFFER_HEIGHT
#define px  BUFFER_RCP_WIDTH
#define py  BUFFER_RCP_HEIGHT

/* Modified Sharpen complex v2 from Media Player Classic Home Cinema */
/* Parameters */

// pour le calcul du flou
#define moyenne 0.6
#define dx (moyenne*px)
#define dy (moyenne*py)

#define CoefFlou 2
#define CoefOri (1+ CoefFlou)

// pour le sharpen
#define SharpenEdge 0.2
#define Sharpen_val0 2
#define Sharpen_val1 ((Sharpen_val0-1) / 8.0)

float4 main( float2 tex ) {

	// recup du pixel original
	float4 ori = tex2D(s0, tex);

	// calcul image floue (filtre gaussien)
	float4 c1 = tex2D(s0, tex + float2(-dx,-dy));
	float4 c2 = tex2D(s0, tex + float2(0,-dy));
	float4 c3 = tex2D(s0, tex + float2(dx,-dy));
	float4 c4 = tex2D(s0, tex + float2(-dx,0));
	float4 c5 = tex2D(s0, tex + float2(dx,0));
	float4 c6 = tex2D(s0, tex + float2(-dx,dy));
	float4 c7 = tex2D(s0, tex + float2(0,dy));
	float4 c8 = tex2D(s0, tex + float2(dx,dy));

	// filtre gaussien
	//   [ 1, 2 , 1 ]
	//   [ 2, 4 , 2 ]
	//   [ 1, 2 , 1 ]
	// pour normaliser les valeurs, il faut diviser par la somme des coef
	// 1 / (1+2+1+2+4+2+1+2+1) = 1 / 16 = .0625
	float4 flou = (c1+c3+c6+c8 + 2*(c2+c4+c5+c7)+ 4*ori)*0.0625;

	// soustraction de l'image flou à l'image originale
	float4 cori = CoefOri*ori - CoefFlou*flou;

	// détection des contours
	// récuppération des 9 voisins
	//   [ c1, c2 , c3 ]
	//   [ c4,ori , c5 ]
	//   [ c6, c7 , c8 ]
	c1 = tex2D(s0, tex + float2(-px,-py));
	c2 = tex2D(s0, tex + float2(0,-py));
	c3 = tex2D(s0, tex + float2(px,-py));
	c4 = tex2D(s0, tex + float2(-px,0));
	c5 = tex2D(s0, tex + float2(px,0));
	c6 = tex2D(s0, tex + float2(-px,py));
	c7 = tex2D(s0, tex + float2(0,py));
	c8 = tex2D(s0, tex + float2(px,py));

	// par filtre de sobel
	// Gradient horizontal
	//   [ -1, 0 ,1 ]
	//   [ -2, 0, 2 ]
	//   [ -1, 0 ,1 ]
	float delta1 =  (c3 + 2*c5 + c8)-(c1 + 2*c4 + c6);

	// Gradient vertical
	//   [ -1,- 2,-1 ]
	//   [  0,  0, 0 ]
	//   [  1,  2, 1 ]
	float delta2 = (c6 + 2*c7 + c8)-(c1 + 2*c2 + c3);

	// calcul
	if (sqrt( mul(delta1,delta1) + mul(delta2,delta2)) > SharpenEdge) {
		// si contour, sharpen
		//return  float4(1,0,0,0);
		return ori*Sharpen_val0 - (c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8 ) * Sharpen_val1;
	} else {
		// sinon, image corrigée
		return cori;
	}
}
__kernel
void Mandelbrot(
	__global unsigned char *output,
	const int w,
	const int h,
	const float cx,
	const float cy,
	const float unit)
{
	int globalID = get_global_id(0);
	int ox = globalID % w;
	int oy = globalID / w;
	int ix = ox;
	int iy = h - 1 - oy;

	if ( ix >= w || iy >= h ) { return; }
	float fx = (float)(ix - w / 2) * unit + cx;
	float fy = (float)(iy - h / 2) * unit + cy;

	float r = fx;
	float i = fy;
	int n;
	int m = 200;
	for (n = 0; n < m; n++) {
		float rr = r * r;
		float ii = i * i;
		float ri = r * i;
		r = fx + rr - ii;
		i = fy + 2 * ri;
		if ( rr + ii > 4 ) { break; }
	}
	float fval = (float)n / (float)m;
	int ival = 256 * fval;
	if (ival < 0) { ival = 0; }
	if (ival > 255) { ival = 255; }

	output[ox + oy * w] = (unsigned char)ival;
}

#declare NPLANETS = NP;

background {
//	color rgbt <.15,.13,.11,1>
	color rgbt <0,0,0,1>
}

light_source {
	<0, 1000, -1000> color rgb <1,1,1>
	rotate z*LSROT
}

camera {
	location <0,0,-4>
	look_at <0,0,0>
	right x
	up y
}

sphere {
	<X1,Y1,Z1>, PSIZE1
	texture {
		TEXTURE1
	}
	rotate x*ROTX
	rotate y*ROTY
	rotate z*ROTZ
}

sphere {
	<X2,Y2,Z2>, PSIZE2
	texture {
		TEXTURE2
	}
	rotate x*ROTX
	rotate y*ROTY
	rotate z*ROTZ
}

#if (NPLANETS > 2)
sphere {
	<X3,Y3,Z3>, PSIZE3
	texture {
		TEXTURE3
	}
	rotate x*ROTX
	rotate y*ROTY
	rotate z*ROTZ
}
#end

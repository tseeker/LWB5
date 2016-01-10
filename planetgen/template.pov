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
	<0,0,0>, PSIZE
	texture {
		TEXTURE
	}
}

#ifdef (RINGS)
disc {
	<0,0,0>, <0,1,0>, RRAD, RHRAD
	no_shadow
	texture {
		pigment {
			function {x*x + z*z}
			color_map {
RCMAP
			}
		}
		finish { ambient 0.6 }
	}
	rotate z*RROTZ
	rotate y*RROTY
}
#end

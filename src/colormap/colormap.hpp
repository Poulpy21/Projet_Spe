
#ifndef COLORMAP_H
#define COLORMAP_H

#include <map>
#define _COLOR_PER_COLORMAP 10

namespace ColorMap {

	typedef float colormap[4*_COLOR_PER_COLORMAP];
	
	std::map<std::string, std::pair<unsigned int, float*> > multiHueColorMaps();

	//green-white
	const colormap multi_hue_1 {
		0,0,0,255,
		247,252,253,255,
		229,245,249,255,
		204,236,230,255,
		153,216,201,255,
		102,194,164,255,
		65,174,118,255,
		35,139,69,255,
		0,109,44,255,
		0,68,27,255
	};
	
	//purple-blue
	const colormap multi_hue_2 {
		0,0,0,255,
		247,252,253,255,
		224,236,244,255,
		191,211,230,255,
		158,188,218,255,
		140,150,198,255,
		140,107,177,255,
		136,65,157,255,
		129,15,124,255,
		77,0,75,255
	};

	//blue-green
	const colormap multi_hue_3 {
		0,0,0,255,
		247,252240,255,
		224,243,219,255,
		204,235,197,255,
		168,221,181,255,
		123,204,196,255,
		78,179,211,255,
		43,140,190,255,
		8,104,172,255,
		8,64,123,255
	};
	
	//red-orange
	const colormap multi_hue_4 {
		0,0,0,255,
		255,247,236,255,
		254,232,200,255,
		253,212,158,255,
		253,187,132,255,
		252,141,89,255,
		239,101,72,255,
		215,48,31,255,
		179,0,0,255,
		127,0,0,255
	};
	
	//blue-white
	const colormap multi_hue_5 {
		0,0,0,255,
		255,247,251,255,
		236,231,242,255,
		208,209,230,255,
		166,189,219,255,
		116,169,207,255,
		54,144,192,255,
		5,112,176,255,
		4,90,141,255,
		2,56,88,255
	};
	
	//green-blue
	const colormap multi_hue_6 {
		0,0,0,255,
		255,247,251,255,
		236,226,240,255,
		208,209,230,255,
		166,189,219,255,
		103,169,207,255,
		54,144,192,255,
		2,129,138,255,
		1,108,89,255,
		1,70,54,255
	};

	//red-purple
	const colormap multi_hue_7 {
		0,0,0,255,
		247,244,249,255,
		231,225,239,255,
		212,185,218,255,
		201,148,199,255,
		223,101,176,255,
		231,41,138,255,
		206,18,86,255,
		152,0,67,255,
		103,0,31,255
	};
	
	//red-pink
	const colormap multi_hue_8 {
		0,0,0,255,
		255,247,243,255,
		253,224,221,255,
		252,197,192,255,
		250,159,181,255,
		247,104,161,255,
		221,52,151,255,
		174,1,126,255,
		122,1,119,255,
		73,0,106,255
	};
	
	//green-yellow
	const colormap multi_hue_9 {
		0,0,0,255,
		255,255,229,255,
		247,252,185,255,
		217,240,163,255,
		173,221,142,255,
		120,198,121,255,
		65,171,93,255,
		35,132,67,255,
		0,104,55,255,
		0,69,41,255
	};
	
	//blue-yellow
	const colormap multi_hue_10 {
		0,0,0,255,
		255,255,217,255,
		237,248,177,255,
		199,233,180,255,
		127,205,187,255,
		65,182,196,255,
		29,145,192,255,
		34,94,168,255,
		37,52,148,255,
		8,29,88,255
	};

	//crimson-yellow
	const colormap multi_hue_11 {
		0,255,255,255,
		255,255,229,255,
		255,247,188,255,
		254,227,145,255,
		254,196,79,255,
		254,153,41,255,
		236,112,20,255,
		204,76,2,255,
		153,52,4,255,
		102,37,6,255
	};
	
	//red-yellow
	const colormap multi_hue_12 {
		0,255,255,255,
		255,255,204,255,
		255,237,160,255,
		254,217,118,255,
		254,178,76,255,
		253,141,60,255,
		252,78,42,255,
		227,26,28,255,
		189,0,38,255,
		128,0,38,255
	};

}


#endif /* end of include guard: COLORMAP_H */

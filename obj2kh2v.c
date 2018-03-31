#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <math.h>
#include <limits>

int flag(int x, int y, int z) {
	int flagx = x;
	if (y > flagx) {flagx = y;}
	if (z > flagx) {flagx = z;} 
	return flagx;
}
int main(int argc, char* argv[]){
	long ap, vp, hp;
	int junk=1;
	printf("obj2kh2v\nPlease input only unwelded sorted models containing only triangles!\n\n");
	if(argc<2){printf("Usage: obj2kh2v model.obj"); return -1;}
	std::ifstream in(argv[1], std::ios::in);
	std::string dsmname = std::string(argv[1]).substr(0,std::string(argv[1]).find_last_of('.'))+".dsm";
	std::string kh2vname = std::string(argv[1]).substr(0,std::string(argv[1]).find_last_of('.'))+".kh2v";
	std::ofstream dsm (dsmname);
	dsm << ".align 0\n;" << argv[1] << "\n;Automatically generated by obj2kh2v\n;DO NOT EDIT IF YOU DON'T KNOW WHAT YOU ARE DOING\n\nstcycl 01, 01; We write code to memory without skips/overwrite\n\nunpack[r] V4_32, 0, *; Model Part Header\n.int 1, 0, 0, 0;type 1 Model\n";
	hp = dsm.tellp();
	//Can't insert if not at eof using ofstream so I put 10 spaces to fill up, size of INT_MAX and no \n to avoid linefeed not beeing rewritten
	//It's so hacky but I'm fed up with this shit already
	dsm << "     " << "          ,           ,           ,                                                                                                                                                                                                                                                                                                           ";
dsm << "\n.EndUnpack\n\nstcycl 01, 01; We write code to memory without skips/overwrite\n\nunpack[r] V2_16, 4, *; UV definition\n";
	int vi =0;
	int ti =0;

	if (dsm.is_open()){
	std::string line;
    while (getline(in, line))
    {
        if (line.substr(0,2) == "v ")
        {
			vi++;
        }
}
in.clear();
in.seekg(0, std::ios::beg);
    while (getline(in, line))
    {
        if (line.substr(0,3) == "vt ")
        {
			std::istringstream s(line.substr(3));
			float u, v;
            s >> u; s >> v;
			dsm << ".short " << int(round(u*4095)) << ", " << int(round(v*4095)) << "\n";        
			ti++;
		}

	}


		dsm << ".EndUnpack\n\nstmask 0xcfcfcfcf; Sets mask register(3303, check EEUSER_E)\nstcycl 01, 01; We write code to memory without skips/overwrite\n\nunpack[r] S_8, 4, *; Vertex indices\n";
for(int i =0; i<vi+1;i++){dsm << ".byte " << i << "\n";}
dsm << ".EndUnpack\n\nstmask 0x3f3f3f3f; Sets mask register(3330, check EEUSER_E)\nstcycl 01, 01; We write code to memory without skips/overwrite\n\nunpack[r] S_8, 4, *; Flags\n";
in.clear();
in.seekg(0, std::ios::beg);
int iff =1;
int rev=0;
int stock=0;
while (getline(in, line))
    {
		//printf("uwot");
        if (line.substr(0,2) == "f ")
        {
line_process:
			std::istringstream s(line.substr(2));
			std::string i, n, u;
			int ii, in, iu;
            s >> i; s >> n; s >> u;
			ii = std::stoi(i.substr(0, i.find("/"))); 			
			in = std::stoi(n.substr(0, n.find("/"))); 			
			iu = std::stoi(u.substr(0, u.find("/"))); 			
			printf("i1: %d i2: %d i3: %d\n", ii, in, iu);		
			printf("flag: %d\n", flag(ii, in, iu));
			if(iff< flag(ii,in,iu)){dsm << ".byte 0x10; stock\n"; stock=1;}
			if(iff == flag(ii,in,iu)){ if(stock==1){stock=0; dsm << ".byte 0x20; draw triangle\n"; rev=0;} else{if(rev==1){dsm << ".byte 0x20; draw triangle\n"; rev=0;} else{dsm << ".byte 0x30; draw reversed triangle\n"; rev=1;}}}
			if(iff > flag(ii,in,iu)){printf("Unordonned Model! Will abort\n"); return -1;}
			iff++;
			printf("iff: %d\n", iff);
			if(stock==1){goto line_process;}

        }
}
dsm << ".EndUnpack\n\nstcol 0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000; We set garbage data to 1(float) so even if nothing is referenced game doesn't go crazy\nstmask 0x80808080; Sets mask register(0002, check EEUSER_E)\nstcycl 01, 01; We write code to memory without skips/overwrite\n\nunpack[r] V3_32,";
vp = dsm.tellp();
dsm << "               , *; Vertex definition\n";

in.clear();
in.seekg(0, std::ios::beg);

  while (getline(in, line))
    {
        if (line.substr(0,2) == "v ")
        {
			std::istringstream s(line.substr(2));
			float x, y, z, w;
            s >> x; s >> y; s >> z; w = 1.0f;
			dsm << ".float " << x << ", " << y << ", " << z << "\n";

        }
}


dsm << ".EndUnpack\n\nstcycl 01, 01; We write code to memory without skips/overwrite\n\nunpack[r] V4_32,";
ap = dsm.tellp();
dsm << "          , *; Vertex affiliation header\n.int " << vi+1 << ", 0, 0, 0\n.EndUnpack\nvifnop\nvifnop; We wait for data to be kicked in\n";

}
printf("h1: %i, h2: 4, h3: %i, h4: %i\nj1: %i, j2: %i, j3: 0, j4: 1\n",ti, 4+ti+vi, 4+ti+vi+1,vi, 4+ti);
		//TODO: Stop hardcode Header size(5type-line exist) and Vert/Array
		//h3: 1 not because 1 value but 1 array of 4, padding needs to be
		//checked!
#if (_WIN32)
		dsm.seekp(hp+10);
#else
		dsm.seekp(hp);
#endif
		dsm << ".int " << ti << ", 4, " << 4+ti+vi+1 << ", " << 4+ti+vi+1 << "; Number of u+v+flag+index, their offset, offset of vertex affiliation header, offset of mat definition(end)\n";
		dsm << ".int 0, 0, 0, 0; Nobody care about vertices merging and colors\n";
        dsm << ".int " << vi+1 << ", " << 4+ti << ", 0, 1; Number of vertex, their offset, reserved and number of array attribution\n";
#if (_WIN32)
		dsm.seekp(vp+10);
#else
		dsm.seekp(vp);
#endif
		dsm << 4+ti;
#if (_WIN32)
		dsm.seekp(ap+10);
#else
		dsm.seekp(ap);
#endif
		dsm << 4+ti+vi;
		dsm.close();
#if (_WIN32)
	if(system(("dvp-as \""+dsmname+"\" -o junk.o").c_str()) != 0){printf("Could not proceed, please install homebrew ps2 sdk!\n"); return -1;}
		if(system(("dvp-objcopy -O binary junk.o \""+kh2vname+"\"").c_str()) != 0){printf("Your homebrew ps2 sdk installation seems to be broken, please reinstall\n"); return -1;}
#else
	if(system(("dvp-as "+dsmname+" -o junk.o > /dev/null").c_str()) != 0){printf("Could not proceed, please install homebrew ps2 sdk!\n"); return -1;}
		if(system(("dvp-objcopy -O binary junk.o "+kh2vname+" > /dev/null").c_str()) != 0){printf("Your homebrew ps2 sdk installation seems to be broken, please reinstall\n"); return -1;}

#endif
		remove("junk.o");
		if(!junk){remove(dsmname.c_str());}
		return 1;
}

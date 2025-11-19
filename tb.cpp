/*
 * App.cpp
 *
 *  Created on: Jan 5, 2022
 *  Author: antonis
 */

#include <iostream>
#include <string>
#include "Conv2D.h"
#include "types.h"
#include "TransferData.h"

int main(){
	
	Kernels* kernels[LN];	
	for(int i=0;i<LN;i++){
		kernels[i] = new Kernels(kfn[i],kd[i],kdepth[i],kn[i]);
	}

	// Create instances to run layers and run the first layer
	FeatureMaps* map1 = new FeatureMaps(X,Y,Z,"padding",1);
	FeatureMaps* bypass = new FeatureMaps(X,Y,Z,"padding",1);
	FeatureMaps* map2 = new FeatureMaps(X,Y,kn[0],"padding",0);
	Conv2D layer(map1,bypass,kernels[0],map2);
	layer.run(true,2);
	cout << "Layer 1 finished\n";

	AvgPool pl(map2,bypass,map1);
	pl.run(true);
	// Run the rest layers using the same instances
	for(int i=1;i<LN;i++){
		TransferData bus(map1,bypass,map2);
		if(i%2==1){
			layer.updateFM(map1,map2);
		}
		if(i%2==0){
			layer.updateFM(map2,map1);
		}
		if(i%2==1 and i!=1){
			bus.add();
		}
		layer.updateKernels(kernels[i]); // update kernels reference
		if(i==7 or i==15 or i==27){
			layer.run(true,2);
		}
		else{
			layer.run(false,1);
		}
		cout << "Layer " << i << " finished.\n";
	}
	// Run average pooling layer
	// AvgPool pl();
	// pl.run();

	cout << "\nProgramm terminated successfully.";

}

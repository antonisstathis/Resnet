/*
 * Kernels.h
 *
 *  Created on: Jan 5, 2022
 *  Author: antonis
 */

//#ifndef KERNELS_H_
//#define KERNELS_H_

#include <iostream>
#include <fstream>
#include <string>
#include <ac_channel.h>

using namespace std;

class Kernels {

private:
		int dimension;
		int depth;
		int NK;
		int NB;
		ac_channel<double> weights;
		ac_channel<double> biases;

public:
	Kernels(int x,int z,int k){

		dimension=x;
		depth=z;
		NK=k;
		NB=k;
		loadKernels();
	}

private:
	void loadKernels() {

		string filePath("kernel1.txt");
		ifstream MyFile(filePath);
		double data=0;

		for(int k=0;k<NK;k++){
			for(int dep=0;dep<depth;dep++) {
				for(int i=0;i<dimension;i++){
					for(int j=0;j<dimension;j++){
						MyFile >> data;
						weights.write(data);
					}
				}
			}
		}

		for(int i=0;i<NK;i++) {
			MyFile >> data;
			biases.write(data);
		}

		MyFile.close();
	}


public:
	double getBias() {
		
		return biases.read();
	}


public:
	double getWeight() {

		return weights.read();
	}

public:
	int getNK() {

		return NK;
	}

public:
	int getKD() {

		return dimension;
	}

public:
	int getDepth() {

		return depth;
	}
	
};


//#endif /* KERNELS_H_ */


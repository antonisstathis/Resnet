/*
 * Conv2D.h
 *
 *  Created on: Jan 5, 2022
 *  Author: antonis
 */

//#ifndef CONV2D_H_
//#define CONV2D_H_

#include <iostream>
#include <string>
#include "FeatureMaps.h"
#include "Kernels.h"
#include <ac_channel.h>

using namespace std;

class Conv2D {

private:
		FeatureMaps* input;
		FeatureMaps* bypass;
		Kernels* kernels;
		FeatureMaps* output;
		// Output dimensions
		int X;
		int Y;
		int Z;
		// Kernels dimenions
		int kn;
		int kd;
		int depth;
		int stride=1;

public:
	Conv2D(FeatureMaps* input,FeatureMaps* bypass,Kernels* kernels,FeatureMaps* output){

		this->input = input;
		this->kernels = kernels;
		// Dimensions of kernels volume
		kn = (*kernels).getNK();
		kd = (*kernels).getKD();
		depth = (*kernels).getDepth();
		// Dimensions of output volume
		X = (*input).getX() - (kd-1);
		Y = (*input).getY() - (kd-1);
		Z = (*kernels).getNK();
		// Set output volume dimensions
		(*output).setX(X);
		(*output).setY(Y);
		(*output).setZ(Z);
		this->output = output;
		this->bypass = bypass;
	}

public:
	void run(boolean bp,int stride) {
		
		// 1. Load dims + adjust output size
	    loadDimensions(stride);
	
	    // Local buffers
	    double*** inp;
	    double**** weights;
	    double* biases;
	    double*** outputs;
	
	    // 2. Allocate buffers based on dynamic dimensions
	    allocateLocalBuffers(inp, weights, biases, outputs);
	
	    // 3. Load input feature maps
	    loadInputs(inp);
	
	    // 4. Load kernel weights
	    loadWeights(weights);
	
	    // 5. Load biases
	    loadBiases(biases);

		int reps=1;
		int nk=0;
		int x=0;
		int y=0;
		int pointY=0;
		int pointX=0;
		int row = 0;
		int column = 0;
		double weight = 0;
		double in = 0;
		double acc = 0;
		double bias = 0;

		while(reps<((X*8)*Y*kn)+1) {

			for(int i=0;i<kd;i++) {
				for(int j=0;j<kd;j++) {
					for(int k=0;k<(*input).getZ();k++) {
						weight = weights[i][j][k][nk];
						in = inp[x][y][k];
						acc = acc + in*weight;
					}
					y++;
				}
				y = pointY;
				x++;
			}
			x = pointX;
			bias = biases[nk];
			acc = acc + bias;

			outputs[row][column][nk] = acc;
			//activationFunc(acc);
			acc = 0;

			column++;
			if(column==Y) {
				row++;
				column = 0;
			}
			if(row==X) {
				row=0;
			}

			reps++;
			if(reps%Y==0) {
				pointY=0;
			}
			else {
				pointY = pointY + stride;
			}
			y = pointY;

			if(reps%Y==0 && pointX<X) {
				pointX = pointX + stride;
			}
			if(pointX==X) {
				pointX=0;
				if(nk<kn)
					nk++;
			}
			x=pointX;
			
			// Write outputs
			if(reps%(X*Y*kn)==0){
				cout << "\nreps=" << reps << "\n";
				for(int i=0;i<X;i++){
					for(int j=0;j<Y;j++){
						for(int k=0;k<Z;k++){
							(*output).writeData(outputs[i][j][k]);
							if(bp==true){
								(*bypass).writeData(outputs[i][j][k]);
							}
						}
					}
				}
				double outputs[X][Y][Z];
			}
		}
	}

	void loadDimensions(int stride) {
	    // Dimensions of kernels volume
	    kn    = kernels->getNK();
	    kd    = kernels->getKD();
	    depth = kernels->getDepth();
	
	    // Dimensions of output volume
	    X = input->getX() - (kd - 1);
	    Y = input->getY() - (kd - 1);
	    Z = kernels->getNK();
	
	    // Set output Z
	    output->setZ(Z);
	
	    this->stride = stride;
	
	    if (stride == 2) {
	        X = X / 2;
	        Y = Y / 2;
	        output->setX(X);
	        output->setY(Y);
	        bypass->setX(X);
	        bypass->setY(Y);
	    }
	}

public:
	void allocateLocalBuffers(double***& inp,
	                          double****& weights,
	                          double*& biases,
	                          double***& outputs)
	{
	    int d1 = input->getX();
	    int d2 = input->getY();
	    int d3 = input->getZ();
	
	    // Allocate 3D inp
	    inp = new double**[d1];
	    for (int i = 0; i < d1; i++) {
	        inp[i] = new double*[d2];
	        for (int j = 0; j < d2; j++) {
	            inp[i][j] = new double[d3];
	        }
	    }
	
	    // Allocate weights[kd][kd][depth][kn]
	    weights = new double***[kd];
	    for (int i = 0; i < kd; i++) {
	        weights[i] = new double**[kd];
	        for (int j = 0; j < kd; j++) {
	            weights[i][j] = new double*[depth];
	            for (int k = 0; k < depth; k++) {
	                weights[i][j][k] = new double[kn];
	            }
	        }
	    }
	
	    // Biases
	    biases = new double[kn];
	
	    // NOTE: your original code has: X = X/8;
	    X = X / 8;
	
	    // Outputs[X][Y][Z]
	    outputs = new double**[X];
	    for (int i = 0; i < X; i++) {
	        outputs[i] = new double*[Y];
	        for (int j = 0; j < Y; j++) {
	            outputs[i][j] = new double[Z];
	        }
	    }
	}


public:
	void loadInputs(double*** inp) {
	    for (int k = 0; k < input->getZ(); k++) {
	        for (int i = 0; i < input->getX(); i++) {
	            for (int j = 0; j < input->getY(); j++) {
	                inp[i][j][k] = input->getData();
	            }
	        }
	    }
	}

public:
	void loadWeights(double**** weights) {
	    for (int n = 0; n < kn; n++) {
	        for (int k = 0; k < depth; k++) {
	            for (int i = 0; i < kd; i++) {
	                for (int j = 0; j < kd; j++) {
	                    weights[i][j][k][n] = kernels->getWeight();
	                }
	            }
	        }
	    }
	}

public:
	void loadBiases(double* biases) {
	    for (int i = 0; i < kn; i++) {
	        biases[i] = kernels->getBias();
	    }
	}

public:
	void updateKernels(Kernels* kernels){

		this->kernels=kernels;
	}

public:
	void updateFM(FeatureMaps* in,FeatureMaps* out){

		input = in;
		output = out;
	}

	void setStride(int s) {

		stride = s;
	}


};

//#endif /* CONV2D_H_ */


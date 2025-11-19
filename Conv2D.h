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
	void run(bool bp, int stride)
	{
	    // 1. Load dimensions and adjust output sizes
	    loadDimensions(stride);
	
	    // 2. Allocate local buffers
	    double***  inp;
	    double**** weights;
	    double*    biases;
	    double***  outputs;
	    allocateLocalBuffers(inp, weights, biases, outputs);
	
	    // 3. Load input, weights, and biases
	    loadInputs(inp);
	    loadWeights(weights);
	    loadBiases(biases);
	
	    // 4. Sliding window convolution loop
	    runConvolutionLoop(weights,inp,biases,outputs,bp);
	}

public:
	void runConvolutionLoop(double**** weights,double*** inp,double* biases,double*** outputs,bool bp) {
		    int reps    = 1;
		    int nk      = 0;
		    int row     = 0;
		    int column  = 0;
		
		    int pointX  = 0;
		    int pointY  = 0;
		
		    int x = 0;
		    int y = 0;
		
		    int totalReps = (X * 8) * Y * kn + 1;
		
		    while (reps < totalReps) {
		
		        // 1. Compute convolution point
		        double acc = computePoint(weights,inp,nk,x,y,kd,depth,biases[nk]);
		
		        // 2. Store result
		        outputs[row][column][nk] = acc;
		
		        // Update output coordinates
		        column++;
		        if (column == Y) {
		            column = 0;
		            row++;
		        }
		        if (row == X)
		            row = 0;
		
		        // 3. Update sliding window pointers
		        updateSlidingWindowCounters(reps,pointX,pointY,x,y,nk,stride,X,Y,kn);
		
		        // 4. Write out results if full block is ready
		        flushOutputsIfNeeded(reps,X,Y,kn,Z,outputs,output,bypass,bp);
		    }
		}

public:
	double computePoint(double**** weights,double*** inp,int nk,     int pointX,  int pointY, int kd,int depth,double bias){
	    double acc = 0.0;
	
	    for (int i = 0; i < kd; i++) {
	        for (int j = 0; j < kd; j++) {
	            for (int k = 0; k < depth; k++) {
	                double in_val  = inp[pointX + i][pointY + j][k];
	                double w_val   = weights[i][j][k][nk];
	                acc += in_val * w_val;
	            }
	        }
	    }
	
	    return acc + bias;
	}

public:
	void updateSlidingWindowCounters(int& reps,int& pointX,int& pointY,int& x,int& y,int& nk,int stride,int X,int Y,int kn) {
	    reps++;
	
	    // Update Y position
	    if (reps % Y == 0)
	        pointY = 0;
	    else
	        pointY += stride;
	
	    y = pointY;
	
	    // Update X position and kernel index
	    if (reps % Y == 0 && pointX < X)
	        pointX += stride;
	
	    if (pointX == X) {
	        pointX = 0;
	        if (nk < kn)
	            nk++;
	    }
	
	    x = pointX;
	}

public:
	void flushOutputsIfNeeded(int reps,int X,int Y,int kn,int Z,double*** outputs,FeatureMaps* output,FeatureMaps* bypass,bool bp) {
	    if (reps % (X * Y * kn) != 0)
	        return;
	
	    for (int i = 0; i < X; i++) {
	        for (int j = 0; j < Y; j++) {
	            for (int k = 0; k < Z; k++) {
	
	                double val = outputs[i][j][k];
	                output->writeData(val);
	                if (bp)
	                    bypass->writeData(val);
	            }
	        }
	    }
	}

public:
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


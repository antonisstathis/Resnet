/*
 * InputVolume.h
 *
 *  Created on: Jan 5, 2022
 *  Author: antonis
 */

//#ifndef INPUTVOLUME_H_
//#define INPUTVOLUME_H_

#include <ac_channel.h>
#include <ac_fixed.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class FeatureMaps {

private:
	int X;
	int Y;
	int Z;
	//double* input;
	ac_channel<double> map;
	string padding;

public:
	FeatureMaps(int x,int y,int z,string pad,int con){

		padding=pad;
		
		if(padding=="no") {
			X = x;
			Y = y;
			Z = z;
			//double in[X][Y][Z];
			//input=in;
			//input = (float*) malloc(X*Y*Z*sizeof(float));
		}
		if(padding=="padding") {
			X=x+2;
			Y=y+2;
			Z=z;
			//double in[X][Y][Z];
			//input=in;
			//input = (float*) malloc(X*Y*Z*sizeof(float));
		}
		
		if(con==1){
			setInput();
		}
	}

public:
	void setInput() {

		string filePath("data.txt");
		ifstream MyFile(filePath);
		double d=0;

		if(padding=="padding") {

			for(int i=0;i<X;i++){
				for(int j=0;j<Y;j++){
					for(int k=0;k<Z;k++){
						if(i!=0 and i!=X-1 and j!=0 and j!=Y-1){
							MyFile >> d;
							map.write(d);
						}
						if(i==0 or i==X-1 or j==0 or j==Y-1){
							map.write(0);
						}
					}
				}
			}
		}

		if(padding=="no") {
			for(int i=0;i<X;i++){
				for(int j=0;j<Y;j++){
					for(int k=0;k<Z;k++){
						MyFile >> d;
						map.write(d);
					}
				}
			}
		}

		MyFile.close();
	}

/*
public:
	double getData(int i,int j,int k) {

		return input[i][j][k];
	}
*/

public:
	void writeData(double data){
	
		map.write(data);
	}

public:
	void setX(int x) {

		X=x;
	}

public:
	void setY(int y) {

		Y=y;
	}

public:
	void setZ(int z) {

		Z=z;
	}

public:
	double getData() {
		
		return map.read();
	}


public:
	int getX() {

		return X;
	}

public:
	int getY() {

		return Y;
	}

public:
	int getZ() {

		return Z;
	}

public:
	void writeFile() {

		double data;
		int counter=0;
		string filePath("out.txt");
		ofstream file(filePath);
		cout << "\n" << (X*Y*Z) << "\n";
		for(int i=0;i<X;i++){
			for(int j=0;j<Y;j++){
				for(int k=0;k<Z;k++){
					data = map.read();
					file << data;
					file << "\n";
					counter+=1;
					cout << "\ncounter=" << counter;
				}
			}
		}
		file.close();

	}

/*
public:
	void printVolume(){

		double data=0;
		for(int k=0;k<Z;k++) {
			for(int i=0;i<X;i++){
				for(int j=0;j<Y;j++){
					data=input[i][j][k];
					cout << data << "\n";
				}
			}
		}

	}
*/

};

//#endif /* INPUTVOLUME_H_ */

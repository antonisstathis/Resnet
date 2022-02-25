class TransferData {

private:
	FeatureMaps* input;
	FeatureMaps* bypass;
	FeatureMaps* output;

public:
	TransferData(FeatureMaps* input,FeatureMaps* bypass,FeatureMaps* output){
		this->input=input;
		this->bypass=bypass;
		this->output=output;
	}

// The add function adds the bypass volume to the output volume of the previous layer and saves the outcome to the input volume for the next layer and to the bypass volume.
public:
	void add(){
		
		int d1 = (*output).getX();
		int d2 = (*output).getY();
		int d3 = (*output).getZ();
		double out[d1][d2][d3];

		int dim1 = (*bypass).getX();
		int dim2 = (*bypass).getY();
		int dim3 = (*bypass).getZ();
		double bp[dim1][dim2][dim3];
	
		for(int k=0;k<(*output).getZ();k++){
			for(int i=0;i<(*output).getX();i++){
				for(int j=0;j<(*output).getY();j++){
					//cout << k << " " << i << " " << j << "\n";
					out[i][j][k] = (*output).getData();
				}
			}
		}

		for(int k=0;k<(*bypass).getZ();k++){
			for(int i=0;i<(*bypass).getX();i++){
				for(int j=0;j<(*bypass).getY();j++){
					//cout << k << " " << i << " " << j << "\n";
					bp[i][j][k] = (*bypass).getData();
				}
			}
		}
		
		for(int k=0;k<d3;k++){
			for(int i=0;i<d1;i++){
				for(int j=0;j<d2;j++){
					inp[i][j][k] = out[i][j][k] + bp[i][j][k];
				}
			}
		}


		for(int k=0;k<d3;k++){
			for(int i=0;i<d1;i++){
				for(int j=0;j<d2;j++){
					(*input).write(inp[i][j][k]);
				}
			}
		}


	}

}

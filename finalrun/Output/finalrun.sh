#!/bin/bash
# Create the Cosine Matrix which is given as input to the svd process
./cosine_main.out

# Run the svd algorithm
# Write code to find the number of rows and colums from the Output file and use it to set the other variables
export PATH=$PATH:/usr/local/projects/graphlab/graphlabapi/release/toolkits/collaborative_filtering/
svd Output --rows=6040 --cols=100 --nsv=100 --nv=100 --max_iter=99 --ncpus=1 --quiet=1 --save_vectors=true --predictions Output --tol=1e-6

# Get the k-rank approximation from the output of the svd
sed -i 1d Outputsingular_values
python readData.py >> kmeansinput.txt

# Run the kmeans algorithm on the output of svd
export PATH=$PATH:/usr/local/projects/graphlab/graphlabapi/release/toolkits/clustering

#for (( c=45; c<=65; c++ ))
#do
    kmeans --data=kmeansinput.txt --clusters=55 --output-clusters=cluster.txt --output-data=data.txt --id=1   
#done

# Run the checkMap and json file generating code on the output of kmeans
# mv data.txt_1_of_1 data.txt
./visualise_cosine.out

#pragma once
struct // class neural_network
{
};

/*
classification problem
1) structure/object<----create network
2) add input layer ????
3) add hidden layers -> specify sizes as vector
4) add output layer ????
5) activation function
6) activation function (derivative)

// following will be passed to fit function
epoch (number of cycles)
learning_rate
regularization parameter

data is in file
load data in x (feature bias inclusive)
load data in y_tmp and make y (2D)

then how x and y will be provided

loading data from file will not be part of fit
we will pass address of a data provider to fit

final call to fit to start training and return model(s)

then call a function with test data to get accuracy score

save model(s)
save score
save network metadata
save cost for generating cost graph

How about a scenario where training is being done across sessions.

check one point what can be done to avoid NAN in case of log
	we have dealt with 0.0 issue
	what about -ve

*/

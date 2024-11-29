#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>

void mean(int n, float *arr, float *aver, float *geom, float *harm); //calculate mean values
float criterion(int n, float *x, float *y, float x_mean, float y_mean); //calculate estimation criterion
void regression(int n, float *x, float *y, int index, float *R2, float *F); //build a regression model
void solve(int n, float *x, float *y, float *a0, float *a1); //solve a system of linear equations
float R2_(int n, float *x, float *y, float a1); //calculate coefficient of determination
float F_(int n, float R2); //calculate fisher criterion
void error(int code); //handle errors

int main()
{
	int n; //number of known data points
	float *x = NULL, *y = NULL; //arrays of known data points coordinates
	float F_cr; //critical value of fisher criterion
	int i, ans = 1; //counters
	while (ans == 1)
	{
		printf("FUNCTION APPROXIMATION\n");
		printf("which set of data points (x,y) do you want to use?\nown -> 1\nby default -> 2\n");
		scanf("%d", &ans);
		if (ans == 1)
		{
			//enter set of data points
			printf("number of data points -> ");
			scanf("%d", &n);
			x = (float*)malloc(n * sizeof(float));
			y = (float*)malloc(n * sizeof(float));
			if (!x || !y)
			{
				error(1);
			}
			for (i = 0; i < n; i++)
			{
				printf("x[%d] -> ", i);
				scanf("%f", &x[i]);
			}
			for (i = 0; i < n; i++)
			{
				printf("y[%d] -> ", i);
				scanf("%f", &y[i]);
			}
			if (n != 9)
			{
				printf("critical value of fisher criterion for the significance level of 0.05 -> ");
				scanf("%f", &F_cr);
			}
			else
			{
				F_cr = 5.59;
			}
		}
		else if (ans == 2)
		{
			//read set of data points from input file
			FILE* p = NULL;
			p = fopen("input.txt", "r");
			if (!p)
			{
				error(2);
			}
			fscanf(p, "%d", &n);
			x = (float*)malloc(n * sizeof(float));
			y = (float*)malloc(n * sizeof(float));
			if (!x || !y)
			{
				error(1);
			}
			for (i = 0; i < n; i++)
			{
				fscanf(p, "%f", &x[i]);
			}
			for (i = 0; i < n; i++)
			{
				fscanf(p, "%f", &y[i]);
			}
			fclose(p);
			F_cr = 5.59;
		}
		float x_aver, x_geom, x_harm;
		float y_aver, y_geom, y_harm;
		//calculate mean values of x and y
		mean(n, x, &x_aver, &x_geom, &x_harm);
		mean(n, y, &y_aver, &y_geom, &y_harm);
		float cr[9]; //estimation criterions for 9 types of regression equations
		printf("%5s | %25s | %8s\n", "index", "equation type", "criterion");
		cr[0] = criterion(n, x, y, x_aver, y_aver);
		printf("%5d | %25s | %8.3f\n", 1, "y = a0 + a1 * x", cr[0]);
		cr[1] = criterion(n, x, y, x_geom, y_aver);
		printf("%5d | %25s | %8.3f\n", 2, "y = a0 + a1 * ln(x)", cr[1]);
		cr[2] = criterion(n, x, y, x_harm, y_aver);
		printf("%5d | %25s | %8.3f\n", 3, "y = a0 + a1 / x", cr[2]);
		cr[3] = criterion(n, x, y, x_aver, y_geom);
		printf("%5d | %25s | %8.3f\n", 4, "y = a0 * a1 ^ x", cr[3]);
		cr[4] = criterion(n, x, y, x_geom, y_geom);
		printf("%5d | %25s | %8.3f\n", 5, "y = a0 * x ^ a1", cr[4]);
		cr[5] = criterion(n, x, y, x_harm, y_geom);
		printf("%5d | %25s | %8.3f\n", 6, "y = e ^ (a0 + a1 / x)", cr[5]);
		cr[6] = criterion(n, x, y, x_aver, y_harm);
		printf("%5d | %25s | %8.3f\n", 7, "y = 1 / (a0 + a1 * x)", cr[6]);
		cr[7] = criterion(n, x, y, x_geom, y_harm);
		printf("%5d | %25s | %8.3f\n", 8, "y = 1 / (a0 + a1 * ln(x))", cr[7]);
		cr[8] = criterion(n, x, y, x_harm, y_harm);
		printf("%5d | %25s | %8.3f\n", 9, "y = x / (a0 + a1 * x)", cr[8]);
		float min_cr = cr[0];
		int index = 0;
		//find the minimum estimation criterion
		for (i = 0; i < 9; i++)
		{
			if (min_cr > cr[i])
			{
				min_cr = cr[i];
				index = i;
			}
		}
		printf("the fittest regression equation: %d\n", index + 1);
		printf("which equation do you want to use?\nindex -> ");
		scanf("%d", &ans);
		float R2, F; //coefficient of determination and fisher criterion
		//build a regression model
		regression(n, x, y, ans - 1, &R2, &F);
		//print characteristics of the regression model
		printf("coefficient of determination: %.3f\nFisher criterion: %.3f\n", R2, F);
		printf("\nCONCLUSION\n");
		printf("%.1f%% of the dependent variable can be explained by the independent variable and %.1f%% by other factors\n", R2 * 100, (1 - R2) * 100);
		if (F > F_cr)
		{
			printf("the regression model is adequate with a reliability of 95%% (probability of error <= 5%%)\n");
		}
		else
		{
			printf("the regression model is not adequate\n");
		}
		free(x);
		free(y);
		printf("\ncontinue?\nyes -> 1\nno -> 2\n");
		scanf("%d", &ans);
	}
	return 0;
}

void mean(int n, float *arr, float *aver, float *geom, float *harm)
{
	*aver = 0, *geom = 1, *harm = 0;
	int i;
	for (i = 0; i < n; i++)
	{
		(*aver) += arr[i];
		(*geom) *= arr[i];
		if (arr[i] != 0)
		{
			(*harm) += 1. / arr[i];
		}
	}
	*aver = *aver / n;
	if (*geom < 0 && n % 2 == 0)
	{
		error(4);
	}
	else if (*geom < 0 && n % 2 != 0)
	{
		*geom = -pow(-(*geom), 1. / n);
	}
	else
	{
		*geom = pow(*geom, 1. / n);
	}
	*harm = n / *harm;
	return;
}

float criterion(int n, float *x, float *y, float x_mean, float y_mean)
{
	int i = 0, flag = 0;
	float y_;
	while (x_mean > x[i])
	{
		if (x_mean == x[i])
		{
			flag = 1;
			break;
		}
		i++;
	}
	if (flag)
	{
		y_ = y[i];
	}
	else
	{
		y_ = y[i - 1] + (y[i] - y[i - 1]) / (x[i] - x[i - 1]) * (x_mean - x[i - 1]);
	}
	return fabs((y_mean - y_) / y_);
}

void regression(int n, float *x, float *y, int index, float *R2, float *F)
{
	int i;
	float a0, a1;
	float *X = NULL, *Y = NULL;
	switch (index)
	{
	case 0:
		solve(n, x, y, &a0, &a1);
		printf("y = %.3f + %.3f * x\n", a0, a1);
		*R2 = R2_(n, x, y, a1);
		break;
	case 1:
		X = (float*)malloc(n * sizeof(float));
		if (!X)
		{
			error(1);
		}
		for (i = 0; i < n; i++)
		{
			if (x[i] <= 0)
			{
				error(4);
			}
			X[i] = log(x[i]);
		}
		solve(n, X, y, &a0, &a1);
		printf("y = %.3f + %.3f * ln(x)\n", a0, a1);
		*R2 = R2_(n, X, y, a1);
		free(X);
		break;
	case 2:
		X = (float*)malloc(n * sizeof(float));
		if (!X)
		{
			error(1);
		}
		for (i = 0; i < n; i++)
		{
			if (x[i] == 0)
			{
				error(4);
			}
			X[i] = 1. / x[i];
		}
		solve(n, X, y, &a0, &a1);
		printf("y = %.3f + %.3f / x\n", a0, a1);
		*R2 = R2_(n, X, y, a1);
		free(X);
		break;
	case 3:
		Y = (float*)malloc(n * sizeof(float));
		if (!Y)
		{
			error(1);
		}
		for (i = 0; i < n; i++)
		{
			if (y[i] <= 0)
			{
				error(4);
			}
			Y[i] = log(y[i]);
		}
		solve(n, x, Y, &a0, &a1);
		printf("y = %.3f * %.3f ^ x\n", exp(a0), exp(a1));
		*R2 = R2_(n, x, Y, a1);
		free(Y);
		break;
	case 4:
		X = (float*)malloc(n * sizeof(float));
		Y = (float*)malloc(n * sizeof(float));
		if (!X || !Y)
		{
			error(1);
		}
		for (i = 0; i < n; i++)
		{
			if (x[i] <= 0 || y[i] <= 0)
			{
				error(4);
			}
			X[i] = log(x[i]);
			Y[i] = log(y[i]);
		}
		solve(n, X, Y, &a0, &a1);
		printf("y = %.3f * x ^ %.3f\n", exp(a0), a1);
		*R2 = R2_(n, X, Y, a1);
		free(X);
		free(Y);
		break;
	case 5:
		X = (float*)malloc(n * sizeof(float));
		Y = (float*)malloc(n * sizeof(float));
		if (!X || !Y)
		{
			error(1);
		}
		for (i = 0; i < n; i++)
		{
			if (x[i] == 0 || y[i] <= 0)
			{
				error(4);
			}
			X[i] = 1. / x[i];
			Y[i] = log(y[i]);
		}
		solve(n, X, Y, &a0, &a1);
		printf("y = e ^ (%.3f + %.3f / x)\n", a0, a1);
		*R2 = R2_(n, X, Y, a1);
		free(X);
		free(Y);
		break;
	case 6:
		Y = (float*)malloc(n * sizeof(float));
		if (!Y)
		{
			error(1);
		}
		for (i = 0; i < n; i++)
		{
			if (y[i] == 0)
			{
				error(4);
			}
			Y[i] = 1. / y[i];
		}
		solve(n, x, Y, &a0, &a1);
		printf("y = 1 / (%.3f + %.3f * x)\n", a0, a1);
		*R2 = R2_(n, x, Y, a1);
		free(Y);
		break;
	case 7:
		X = (float*)malloc(n * sizeof(float));
		Y = (float*)malloc(n * sizeof(float));
		if (!X || !Y)
		{
			error(1);
		}
		for (i = 0; i < n; i++)
		{
			if (x[i] <= 0 || y[i] == 0)
			{
				error(4);
			}
			X[i] = log(x[i]);
			Y[i] = 1. / y[i];
		}
		solve(n, X, Y, &a0, &a1);
		printf("y = 1 / (%.3f + %.3f * ln(x))\n", a0, a1);
		*R2 = R2_(n, X, Y, a1);
		free(X);
		free(Y);
		break;
	case 8:
		X = (float*)malloc(n * sizeof(float));
		Y = (float*)malloc(n * sizeof(float));
		if (!X || !Y)
		{
			error(1);
		}
		for (i = 0; i < n; i++)
		{
			if (x[i] == 0 || y[i] == 0)
			{
				error(4);
			}
			X[i] = 1. / x[i];
			Y[i] = 1. / y[i];
		}
		solve(n, X, Y, &a0, &a1);
		printf("y = x / (%.3f + %.3f * x)\n", a1, a0);
		*R2 = R2_(n, X, Y, a1);
		free(X);
		free(Y);
		break;
	}
	*F = F_(n, *R2);
}

void solve(int n, float *x, float *y, float *a0, float *a1)
{
	int i = 0;
	float x_sum = 0, y_sum = 0, xy_sum = 0, x2_sum = 0;
	for (i = 0; i < n; i++)
	{
		x_sum += x[i];
		y_sum += y[i];
		xy_sum += x[i] * y[i];
		x2_sum += pow(x[i], 2);
	}
	*a1 = (xy_sum - 1. / n * x_sum * y_sum) / (x2_sum - 1. / n * pow(x_sum, 2));
	*a0 = 1. / n * y_sum - *a1 / n * x_sum;
	return;
}

float R2_(int n, float *x, float *y, float a1)
{
	int i;
	float r2;
	float x_sum = 0, y_sum = 0, xy_sum = 0, y2_sum = 0;
	for (i = 0; i < n; i++)
	{
		x_sum += x[i];
		y_sum += y[i];
		xy_sum += x[i] * y[i];
		y2_sum += pow(y[i], 2);
	}
	r2 = (a1 * (xy_sum - 1. / n * x_sum * y_sum)) / (y2_sum - 1. / n * pow(y_sum, 2));
	return r2;
}

float F_(int n, float R2)
{
	return R2 / (1 - R2) * (n - 2);
}

void error(int code)
{
	switch (code)
	{
	case 1:
		printf("memory allocation error\n");
		exit(1);
	case 2:
		printf("file opening error\n");
		exit(2);
	case 3:
		printf("invalid calculations\n");
		exit(4);
	}
}

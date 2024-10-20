/*
Author: David Holmqvist <daae19@student.bth.se>
*/

#include "filters.hpp"
#include "matrix.hpp"
#include "ppm.hpp"
#include <cmath>

//New for task 3: Add pthreads to do parallelization
#include <pthread.h>

//Example, will make better 
//Synchronization stuff like mutex locks shouldn't be necessary since it's irrelevant if one thread is working faster than the other (I think, lol)
//int threadCount = 16;

//Struct to store data between threads
struct ThreadData
{
	//So for input and output, we'll need to store matrices
	Matrix* inputMatrix = NULL;
	Matrix* outputMatrix = NULL;
	
	int beginX;
	int endX;
	
	//ySize for this thread
	int beginY;
	int endY;

    //Grabbing the radius and the weight-array as well
    int r;
    double *w;
};

//Functions to pass in to thread_create
void* BlurX(void* args)
{
	ThreadData *data = (ThreadData*) args;
	
	//Get variables from ThreadData
    Matrix* input = data->inputMatrix;  //toReturn
    Matrix* output = data->outputMatrix;//Scratch
	int xSize = data->endX;
	int ySize = data->endY;
    int radius = data->r;
	
	for (int x = data->beginX; x < xSize; x++)
    {
        for (int y = data->beginY; y < ySize; y++)
        {
            double r = data->w[0] * input->r(x, y);
            double g = data->w[0] * input->g(x, y);
            double b = data->w[0] * input->b(x, y);
            double n = data->w[0];

            for (int wi = 1; wi <= radius; wi++)
            {
                double wc = data->w[wi];
                int x2 = x - wi;
                if (x2 >= 0)
                {
                    r += wc * input->r(x2, y);
                    g += wc * input->g(x2, y);
                    b += wc * input->b(x2, y);
                    n += wc;
                }
                x2 = x + wi;
                if (x2 < xSize)
                {
                    r += wc * input->r(x2, y);
                    g += wc * input->g(x2, y);
                    b += wc * input->b(x2, y);
                    n += wc;
                }
            }
            output->r(x, y) = r / n;
            output->g(x, y) = g / n;
            output->b(x, y) = b / n;
        }
    }
	
	pthread_exit(NULL); //Have the thread exit once it's done its part
}

void* BlurY(void* args)
{
	ThreadData *data = (ThreadData*) args;
	
	//Get variables from ThreadData
    Matrix* input = data->inputMatrix;  //Scratch
    Matrix* output = data->outputMatrix;//toReturn
	int xSize = data->endX;
	int ySize = data->endY;
    int radius = data->r;
	
	for (int x = data->beginX; x < xSize; x++)
    {
        for (int y = data->beginY; y < ySize; y++)
        {
            double r = data->w[0] * input->r(x, y);
            double g = data->w[0] * input->g(x, y);
            double b = data->w[0] * input->b(x, y);
            double n = data->w[0];

            for (int wi = 1; wi <= radius; wi++)
            {
                double wc = data->w[wi];
                int y2 = y - wi;
                if (y2 >= 0)
                {
                    r += wc * input->r(x, y2);
                    g += wc * input->g(x, y2);
                    b += wc * input->b(x, y2);
                    n += wc;
                }
                y2 = y + wi;
                if (y2 < xSize)
                {
                    r += wc * input->r(x, y2);
                    g += wc * input->g(x, y2);
                    b += wc * input->b(x, y2);
                    n += wc;
                }
            }
            output->r(x, y) = r / n;
            output->g(x, y) = g / n;
            output->b(x, y) = b / n;
        }
    }
	
	pthread_exit(NULL); //Have the thread exit once it's done its part
}


namespace Filter
{

    namespace Gauss
    {
        void get_weights(int n, double *weights_out)
        {
            for (auto i{0}; i <= n; i++)
            {
                double x{static_cast<double>(i) * max_x / n};
                weights_out[i] = exp(-x * x * pi);
            }
        }
    }
	
	Matrix blurParallel(Matrix m, const int radius, int threadCount)
	{
		Matrix scratch(PPM::max_dimension);
		Matrix toReturn{m};
		
		//1. Moved out of both of the nested loops because it only needs to be calculated once
		double weights[Gauss::max_radius]{};
		Gauss::get_weights(radius, weights);
		
		//2. Taken out of every for-loop conditional to reduce unnecessary function calls
		int sizeX = toReturn.get_x_size();
		int sizeY = toReturn.get_y_size();
		
		//Multi-threading time
		pthread_t threads[threadCount];
		ThreadData threadData[threadCount];
		
		//Get how much one thread is supposed to work on
        //So let's instead just split down the middle of the image in Y
		//int chunkSizeX = sizeX / threadCount;
		int chunkSizeY = sizeX / threadCount;
		
		//Blur X
		for(int i = 0; i < threadCount; i++)
		{
			//Input and output matrices
			threadData[i].inputMatrix = &toReturn;
			threadData[i].outputMatrix = &scratch;
			
            //X
            threadData[i].beginX = 0;
            threadData[i].endX = sizeX;
			
            //Y
            threadData[i].beginY = chunkSizeY * i; //First thread Y begins at 0, second thread Y begins at 381
			threadData[i].endY = (i == threadCount - 1) ? sizeY : (i + 1) * chunkSizeY; //First thread Y ends at 381, second thread Y ends at 763

            //Additional variables needed
            threadData[i].r = radius;
            threadData[i].w = weights;
			
			//Per-thread BlurX
            pthread_create(
                &threads[i],
                NULL,
                BlurX,
                (void*)&threadData[i]
            );
		}

        //Make sure that the threads are done with BlurX before moving on to BlurY
        for(int i = 0; i < threadCount; i++)
        {
            pthread_join(threads[i], NULL);
        }
		
		//Blur Y
		for(int i = 0; i < threadCount; i++)
		{
			threadData[i].inputMatrix = &scratch;
			threadData[i].outputMatrix = &toReturn; 
			
            threadData[i].beginX = 0;
            threadData[i].endX = sizeX;

			threadData[i].beginY = chunkSizeY * i;
			threadData[i].endY = (i == threadCount - 1) ? sizeY : (i + 1) * chunkSizeY;
			
			//Per-thread BlurY
            pthread_create(
                &threads[i],
                NULL,
                BlurY,
                (void*)&threadData[i]
            );
		}

        //Same as above, ensure that the threads are done with BlurY before we return out of the function
        for(int i = 0; i < threadCount; i++)
        {
            pthread_join(threads[i], NULL);
        }

        return toReturn;
	}

    Matrix blur(Matrix m, const int radius)
    {
        Matrix scratch{PPM::max_dimension};
        auto dst{m};
		
		//1. Moved out of both of the nested loops because it only needs to be calculated once
		double w[Gauss::max_radius]{};
        Gauss::get_weights(radius, w);
		
		//2. Taken out of every for-loop conditional to reduce unnecessary function calls
		int xSize = dst.get_x_size();
		int ySize = dst.get_y_size();

        for (auto x{0}; x < xSize; x++)
        {
            for (auto y{0}; y < ySize; y++)
            {
                // unsigned char Matrix::r(unsigned x, unsigned y) const
                // {
                //     return R[y * x_size + x];
                // }

                auto r{w[0] * dst.r(x, y)}, g{w[0] * dst.g(x, y)}, b{w[0] * dst.b(x, y)}, n{w[0]};

                for (auto wi{1}; wi <= radius; wi++)
                {
                    auto wc{w[wi]};
                    auto x2{x - wi};
                    if (x2 >= 0)
                    {
                        r += wc * dst.r(x2, y);
                        g += wc * dst.g(x2, y);
                        b += wc * dst.b(x2, y);
                        n += wc;
                    }
                    x2 = x + wi;
                    if (x2 < xSize)
                    {
                        r += wc * dst.r(x2, y);
                        g += wc * dst.g(x2, y);
                        b += wc * dst.b(x2, y);
                        n += wc;
                    }
                }
                scratch.r(x, y) = r / n;
                scratch.g(x, y) = g / n;
                scratch.b(x, y) = b / n;
            }
        }

        for (auto x{0}; x < xSize; x++)
        {
            for (auto y{0}; y < ySize; y++)
            {
                auto r{w[0] * scratch.r(x, y)}, g{w[0] * scratch.g(x, y)}, b{w[0] * scratch.b(x, y)}, n{w[0]};

                for (auto wi{1}; wi <= radius; wi++)
                {
                    auto wc{w[wi]};
                    auto y2{y - wi};
                    if (y2 >= 0)
                    {
                        r += wc * scratch.r(x, y2);
                        g += wc * scratch.g(x, y2);
                        b += wc * scratch.b(x, y2);
                        n += wc;
                    }
                    y2 = y + wi;
                    if (y2 < ySize)
                    {
                        r += wc * scratch.r(x, y2);
                        g += wc * scratch.g(x, y2);
                        b += wc * scratch.b(x, y2);
                        n += wc;
                    }
                }
                dst.r(x, y) = r / n;
                dst.g(x, y) = g / n;
                dst.b(x, y) = b / n;
            }
        }

        return dst;
    }

}

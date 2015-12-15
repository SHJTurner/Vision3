#include "particleFilter.hpp"

#define dim 3
#define nParticles 100

particleFilter::particleFilter()
{

}

particleFilter::particleFilter(float xMaxRange,float yMaxRange, float zMaxRange, float xMinRange, float yMinRange, float zMinRange)
{
    this->minRange[0] = xMinRange;
    this->minRange[1] = yMinRange;
    this->minRange[2] = zMinRange;
    this->maxRange[0] = xMaxRange;
    this->maxRange[1] = yMaxRange;
    this->maxRange[2] = zMaxRange;
    this->Range[0] = xMaxRange - xMinRange;
    this->Range[1] = yMaxRange - yMinRange;
    this->Range[2] = zMaxRange - zMinRange;
    cvInitMatHeader(&this->LB, 3, 1, CV_32FC1, this->minRange);
    cvInitMatHeader(&this->UB, 3, 1, CV_32FC1, this->maxRange);
    this->condens = cvCreateConDensation(dim, dim, nParticles);
    cvConDensInitSampleSet(this->condens, &this->LB, &this->UB);

    // we're just using a 3x3 identity matrix.
    this->condens->DynamMatr[0] = 1.0;
    this->condens->DynamMatr[1] = 0.0;
    this->condens->DynamMatr[2] = 0.0;

    this->condens->DynamMatr[3] = 0.0;
    this->condens->DynamMatr[4] = 1.0;
    this->condens->DynamMatr[5] = 0.0;

    this->condens->DynamMatr[6] = 0.0;
    this->condens->DynamMatr[7] = 0.0;
    this->condens->DynamMatr[8] = 1.0;
}

cv::Point3f particleFilter::filter(float x, float y, float z)
{
    for (int i = 0; i < this->condens->SamplesNum; i++) //for all particles
    {
        float diffX = std::abs(x - this->condens->flSamples[i][0])/this->maxRange[0]; //maxRange[0] should be x range. Works as ling minRange[0] = 0
        float diffY = std::abs(y - this->condens->flSamples[i][1])/this->maxRange[1]; //maxRange[1] should be y range
        float diffZ = std::abs(z - this->condens->flSamples[i][2])/this->maxRange[2]; //maxRange[1] should be z range

        condens->flConfidence[i] = 1.0 / (sqrt(diffX * diffX + diffY * diffY + diffZ * diffZ)); //update confidence for each Sample
    }

    cvConDensUpdateByTime(condens); //Estimates the subsequent stochastic model state from its current state

    return cv::Point3f(condens->State[0], condens->State[1],condens->State[2]);
}

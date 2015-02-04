// niblack.cpp

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp> 
#include "niblack.h"

using namespace std;
using namespace cv;

int prev__niblack( cv::Mat1b& src,
                   cv::Mat1b& res,
                   int wing_min /* = 5*/,
                   double alpha /*= 0.15*/, 
                   bool invert )
{
    res = src.clone();
    // int wing_min = 5;
    int wing_max = wing_min; // *8;

    Mat sum, sumsq;
    cv::integral( src, sum, sumsq );

    double avesigma = 0;
    int avesigma_cnt = 0;

    Mat e( src.rows, src.cols, CV_8U ); // average at [y x]
    Mat d( src.rows, src.cols, CV_8U ); // sigma at [y x]

    int object_value = invert ? 255 : 0;
    int background_value = invert ? 0 : 255;

    for ( int y = 0; y < src.rows; ++y )
    {
        for ( int x = 0; x < src.cols; ++x )
        {
            for ( int wing = wing_min; wing <= wing_max; wing*=2 )
            {
                int sizex = min(wing, x) + min(wing, src.cols - x - 1) + 1;
                int sizey = min(wing, y) + min(wing, src.rows - y - 1) + 1;
                double size = sizex * sizey;

                Point from = Point(max(0, x - wing), max(0, y - wing));
                Point to = Point(min(src.cols, x + wing + 1), min(src.rows, y + wing + 1));
                int s = sum.at<int>(to.y, to.x) - sum.at<int>(from.y, to.x) - sum.at<int>(to.y, from.x) + sum.at<int>(from.y, from.x);
                double sq = sumsq.at<double>(to.y, to.x) - sumsq.at<double>(from.y, to.x) - sumsq.at<double>(to.y, from.x) + sumsq.at<double>(from.y, from.x);

                double E = double(s) / size;
                e.at<uchar>(y,x) = saturate_cast<uchar>( E );

                double sigma = sqrt( (double(sq) / size) - E*E );
                d.at<uchar>( y, x ) = saturate_cast<uchar>( sigma );

                avesigma += sigma;
                ++avesigma_cnt;

                /// double alpha = 0.2;

                // ???
                if ( alpha * sigma < 4 && (wing/wing_min) >= 4 )
                {
                    res[y][x] = E > 128 ? 255 : 64; /////(128+64) : 64;
                    break;
                }

                int T1 = int( E - alpha * sigma + 0.5 );
                int T2 = int( E + alpha * sigma + 0.5 );

                int srcxy = src[y][x];

                if ( srcxy < T1 )
                {
                    res[y][x] = background_value;
                    break;
                }
                else if ( srcxy > T2 )
                {
                    res[y][x] = object_value;
                    break;
                }
                else
                {
                    res[y][x] = 128;
                }

            }
        }
    }
    cv::imwrite( "res.jpg", res );

    avesigma /= avesigma_cnt;
//     printf( "avesigma = %f", avesigma );

    Mat dbin( src.rows, src.cols, CV_8U );
    double thr1 = threshold(d, dbin, 0, 255, THRESH_OTSU);
//     printf("Thresh sigma %f", thr1 );

    dbin = 255 - dbin;
//     imshow( "blocks", dbin );
//     imwrite( "blocks.jpg", dbin );

    ///double alpha = 0.15; // 0.2;

    for ( int y = 0; y < src.rows; ++y )
    {
        for ( int x = 0; x < src.cols; ++x )
        {
            int ee = e.at<uchar>(y, x);
            int dd = d.at<uchar>(y, x);
            if (dd < thr1)
            {
                res[y][x] = background_value; // 200;
                continue;
            }

            int srcxy = src[y][x];
            int T1 = int( ee - alpha * dd + 0.5 );
            int T2 = int( ee + alpha * dd + 0.5 );
            res[y][x] = (srcxy > T1) ? object_value : background_value;
        }
    }

    //imshow( "binarized niblack ex", res );
    //waitKey(0);

    return 0;
}


int niblack( cv::Mat1b& src, cv::Mat1b& res,
             int threshold_value,
             int wing /*= 5 */, 
             double alpha /*= 0.15*/,
             bool invert )
{
    int object_value = invert ? 255 : 0;
    int background_value = invert ? 0 : 255;

    Mat sum, sumsq;
    cv::integral( src, sum, sumsq );

    double avesigma = 0;
    int avesigma_cnt = 0;

    // pass one -- thresh sigma
    Mat e( src.rows, src.cols, CV_8U ); // average at [y x]
    Mat d( src.rows, src.cols, CV_8U ); // sigma at [y x]

    for ( int y = 0; y < src.rows; ++y )
    {
        for ( int x = 0; x < src.cols; ++x )
        {
            int sizex = min(wing, x) + min(wing, src.cols - x - 1) + 1;
            int sizey = min(wing, y) + min(wing, src.rows - y - 1) + 1;
            double size = sizex * sizey;

            Point from = Point(max(0, x - wing), max(0, y - wing));	
            Point to = Point(min(src.cols, x + wing + 1), min(src.rows, y + wing + 1));
            int s = sum.at<int>(to.y, to.x) - sum.at<int>(from.y, to.x) - sum.at<int>(to.y, from.x) + sum.at<int>(from.y, from.x);
            double sq = sumsq.at<double>(to.y, to.x) - sumsq.at<double>(from.y, to.x) - sumsq.at<double>(to.y, from.x) + sumsq.at<double>(from.y, from.x);

            double E = double(s) / size;
            e.at<uchar>(y,x) = saturate_cast<uchar>( E );

            double sigma = sqrt( (double(sq) / size) - E*E );
            d.at<uchar>(y,x) = saturate_cast<uchar>( sigma );

            avesigma += sigma;
            avesigma_cnt++;
        }
    }

    // res.create( src.rows, src.cols );
    cv::Mat1b tempResult( src.rows, src.cols );
    tempResult.setTo( invert ? 0 : 255 );
    cv::imwrite( "dispersion.jpg", d );
    double thr1 = threshold(d, tempResult, threshold_value, 255 /*200*/, /*THRESH_OTSU |*/ THRESH_BINARY );
//     printf("Thresh sigma %f \n", thr1 );
//     imshow( "blocks", tempResult );
    cv::imwrite( "blocks.jpg", tempResult );

   
    wing *= 3;

    for ( int y = 0; y < src.rows; ++y )
    {
        for ( int x = 0; x < src.cols; ++x )
        {
            if ( tempResult[y][x] == object_value )
            {
                int sizex = min(wing, x) + min(wing, src.cols - x - 1) + 1;
                int sizey = min(wing, y) + min(wing, src.rows - y - 1) + 1;
                double size = sizex * sizey;

                Point from = Point(max(0, x - wing), max(0, y - wing));
                Point to = Point(min(src.cols, x + wing + 1), min(src.rows, y + wing + 1));
                int s = sum.at<int>(to.y, to.x) - sum.at<int>(from.y, to.x) - sum.at<int>(to.y, from.x) + sum.at<int>(from.y, from.x);
                double sq = sumsq.at<double>(to.y, to.x) - sumsq.at<double>(from.y, to.x) - sumsq.at<double>(to.y, from.x) + sumsq.at<double>(from.y, from.x);

                double E = double(s) / size;
                double sigma = sqrt( (double(sq) / size) - E*E );
                uchar srcxy = src[y][x];
                bool is_background = ( invert && 255 - src[y][x] > 255 - E - alpha*sigma && src[y][x] < 5 )
                                  || ( !invert && src[y][x] > E - alpha*sigma && src[y][x] > 250 );
                if ( is_background )
                {
                    tempResult[y][x] = background_value;
                }
            }
        }
    }

    tempResult.copyTo( res );
    return 0;
}



int niblack_ex( cv::Mat1b& src, cv::Mat1b& res, bool invert )
{
    int object_value = invert ? 255 : 0;
    int background_value = invert ? 0 : 255;

    res = src.clone();
    int wing_min = 7;
    int wing_max = wing_min; // *8;

    Mat sum, sumsq;
    cv::integral( src, sum, sumsq );

    double avesigma = 0;
    int avesigma_cnt = 0;

    Mat e( src.rows, src.cols, CV_8U ); // average at [y x]
    Mat d( src.rows, src.cols, CV_8U ); // sigma at [y x]

    for ( int y = 0; y < src.rows; ++y )
    {
        for ( int x = 0; x < src.cols; ++x )
        {
            for ( int wing = wing_min; wing <= wing_max; wing*=2 )
            {
                int sizex = min(wing, x) + min(wing, src.cols - x - 1) + 1;
                int sizey = min(wing, y) + min(wing, src.rows - y - 1) + 1;
                double size = sizex * sizey;

                Point from = Point(max(0, x - wing), max(0, y - wing));
                Point to = Point(min(src.cols, x + wing + 1), min(src.rows, y + wing + 1));
                int s = sum.at<int>(to.y, to.x) - sum.at<int>(from.y, to.x) - sum.at<int>(to.y, from.x) + sum.at<int>(from.y, from.x);
                double sq = sumsq.at<double>(to.y, to.x) - sumsq.at<double>(from.y, to.x) - sumsq.at<double>(to.y, from.x) + sumsq.at<double>(from.y, from.x);

                double E = double(s) / size;
                e.at<uchar>(y,x) = saturate_cast<uchar>( E );

                double sigma = sqrt( (double(sq) / size) - E*E );
                d.at< uchar >(y,x) = saturate_cast<uchar>( sigma );

                avesigma += sigma;
                avesigma_cnt++;

                double koeff = 0.2;
/*
                // ???
                if ( koeff * sigma < 4 && (wing/wing_min) >= 4 )
                {
                    res[y][x] = E > 128 ? 255 : 64; /////(128+64) : 64;
                    break;
                }

                int T1 = int( E - koeff * sigma + 0.5 );
                int T2 = int( E + koeff * sigma + 0.5 );

                int srcxy = src[y][x];

                if ( srcxy < T1 )
                {
                    res[y][x] = object_value;
                    break;
                }
                else if ( srcxy > T2 )
                {
                    res[y][x] = background_value;
                    break;
                }
                else
                {
                  res[y][x] = 128;
                }
*/
            }
        }
    }

    avesigma /= avesigma_cnt;
//     printf( "avesigma = %f", avesigma );

    Mat dbin( src.rows, src.cols, CV_8U ); 
    double thr1 = threshold(d, dbin, 0, 255, THRESH_OTSU);
//     printf("Thresh sigma %f", thr1 );

//    dbin = 255 - dbin;
//     imshow( "blocks", dbin );

    double koeff = 0.2;

    for ( int y = 0; y < src.rows; ++y )
    {
        for ( int x = 0; x < src.cols; ++x )
        {
            int ee = e.at<uchar>(y, x);
            int dd = d.at<uchar>(y, x);
            if ( dd < thr1 )
            {
                res[y][x] = background_value; // 200;
                continue;
            }

            int srcxy = src[y][x];
            int T1 = int( ee - koeff * dd + 0.5 );
            int T2 = int( ee + koeff * dd + 0.5 );
//             T2 = std::max( T2, 64 );
            res[y][x] = (srcxy > T2) ? object_value : background_value;
        }
    }

    return 0;
}



//////////////////////////////////////////////////////////////////////////


static void CountImageIntegrals( const cv::Mat& inputImage, cv::Mat& sum, cv::Mat& sqsum )
{
    cv::Mat image( inputImage.rows, inputImage.cols, CV_64FC1 );
    for( int x = 0; x < image.rows; ++x )
    {
        for( int y = 0; y < image.cols; ++y )
        {
            image.at<double>( x, y ) = inputImage.at<uchar>( x, y ) / 255.0;
        }
    }
    cv::integral( image, sum, sqsum );
}

static int CountThreshold( int x, int y, int width, int height,
                          const cv::Mat& sum, const cv::Mat& sqsum, double k )
{ 
    int numPixel = width * height;

    double M = ( sum.at<double>( y, x ) - sum.at<double>( y, x + width )
        - sum.at<double>( y + height, x ) + sum.at<double>( y + height, x + width ) ) / numPixel;
    double M2 = ( sqsum.at<double>( y, x ) - sqsum.at<double>( y, x + width ) 
        - sqsum.at<double>( y + height, x ) + sqsum.at<double>( y + height, x + width ) ) / numPixel;
    double D = std::sqrt(( numPixel * std::abs( M2 - M * M ) ) / (numPixel - 1));

    int thresh = static_cast<int>( 255 * ( M - k * D ) ); 
    return thresh;

}

int niblack_standart( cv::Mat1b& image, cv::Mat1b& res, int size, double k, int threshold, bool invert )
{
    int object_value = invert ? 255 : 0;
    int background_value = invert ? 0 : 255;

    cv::Mat sum, sqsum;
    CountImageIntegrals( image, sum, sqsum );

    if( size > image.cols || size > image.rows ) {
        size = std::min( image.cols, image.rows );
    }

    cv::Mat binImage( image.rows, image.cols, CV_8UC1 );
    for( int j = 0; j < image.rows; ++j )
    {
        for( int i = 0 ; i < image.cols; ++i )
        {
            int x = std::max( i - size / 2, 0 );
            int width = std::min( i + size / 2, image.cols - 1 ) - x - 1;
            int y = std::max( j - size / 2, 0 );
            int height = std::min( j + size/2, image.rows - 1 ) - y - 1;

            int T = CountThreshold( x + 1, y + 1, width, height, sum, sqsum, k ) + threshold;
            uchar value = image.at<uchar>( j, i );
            if( ( invert && value > T ) || ( !invert && value < T ) )
            {
                binImage.at<uchar>( j, i ) = object_value;
            }
            else 
            {
                binImage.at<uchar>( j, i ) = background_value;
            }
        }
    }

    binImage.copyTo( res );
    return 0;
}
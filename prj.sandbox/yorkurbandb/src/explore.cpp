// explore.cpp

#include "imagerecord.h"


using namespace std;
using namespace cv;

//#include "cover_net.h"



void ImageRecord::explore()
{
  
  {//////////// show segments
    string imagename = name + ".jpg";
    Mat mat = imread( imagename );
    for (int i=0; i< int(segments.size()); i++)
    {
      line( mat, segments[i].first, segments[i].second, Scalar( 100, 0, 200 ) );
    }

    imshow( "segments", mat );
    waitKey(0);
  }

  for (int i=0; i< int(segments.size()); i++)
  {
    HLine3d hline;
    if (hcoords.segment2hline( segments[i].first, segments[i].second, hline )) // отрезок не вырожден?
      hlines.push_back( hline );
  }

  {//////////// show hlines
    string imagename = name + ".jpg";
    Mat mat = imread( imagename );
    for (int i=0; i< int(hlines.size()); i++)
    {
      Point p1,p2; 
      hcoords.hline2points( hlines[i], p1, p2 );
      line( mat, p1, p2, Scalar( 200, 0, 200 ) );
    }

    imshow( "hlines", mat );
    waitKey(0);
  }


  //////////// show hlines

  for (int i=0; i< int(hlines.size()); i++)
  {
    cout << "hline: " << hlines[i] << endl;
    cout << "length: " << length( hlines[i] ) << endl;
  }

  ////Metr1 ruler;  CoverTree< int, Metr1 > tree( &ruler, 1000, 1 );
  //Metr2 ruler;  CoverTree< int, Metr2 > tree( &ruler, 1000, 1 );

  //for (int i=0; i< int( samples.size() ); i++)
  //  tree.insert( i );

  //tree.reportStatistics( 0, 3 ); 

}
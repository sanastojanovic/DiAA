#include "pbPlots.hpp"
#include "supportLib.hpp"
#include "visualizer.hpp"
#include <iostream>

// Drawing
void visualize(std::vector <Point> points, std::vector <std::pair <int, int>> edges, int W, int H, std::string file_name){

    bool success;

    StringReference *errorMessage = CreateStringReferenceLengthValue(0, L' ');
    RGBABitmapImageReference *imageReference = CreateRGBABitmapImageReference();

    int n = points.size();
    std::vector <double> xs, ys;
    for(int i=0; i<n; i++){
        xs.push_back(points[i].x());
        ys.push_back(points[i].y());
    }

    ScatterPlotSeries *series = GetDefaultScatterPlotSeriesSettings();
    series->xs = &xs;
    series->ys = &ys;
    series->linearInterpolation = false;
    series->pointType = toVector(L"dots");
    series->color = CreateRGBColor(0, 0, 0);

    ScatterPlotSettings *settings = GetDefaultScatterPlotSettings();
    settings->width = W;
    settings->height = H;
    settings->autoBoundaries = false;
    settings->xMax = W;
    settings->xMin = 0;
    settings->yMax = H;
    settings->yMin = 0;
    settings->autoPadding = false;
    settings->xPadding = 0;
    settings->yPadding = 0;
    settings->title = toVector(L"");
    settings->xLabel = toVector(L"");
    settings->yLabel = toVector(L"");
    settings->scatterPlotSeries->push_back(series);
    settings->showGrid = false;

    success = DrawScatterPlotFromSettings(imageReference, settings, errorMessage);

    if(success){
        double point_size = 10;

        std::cout << "Drawing edges" << std::endl;

        int progress = 1;

        for(int id=0; id<edges.size(); id++){
            if(id == (edges.size()/10) * progress){
                std::cout << "Edges " << (100*id / edges.size()) << "% done" << std::endl;
                progress++;
            }

            auto c = edges[id];
            Point p1 = points[c.first];
            Point p2 = points[c.second];

            double x1 = p1.x(), y1 = p1.y();
            double x2 = p2.x(), y2 = p2.y();

            double dx1 = MapXCoordinateBasedOnSettings(x1, settings) - 1;
            double dy1 = MapYCoordinateBasedOnSettings(y1, settings) - 0.1;
            double dx2 = MapXCoordinateBasedOnSettings(x2, settings) - 1;
            double dy2 = MapYCoordinateBasedOnSettings(y2, settings) - 0.1;

            if(rand()%2){
                DrawLine(imageReference->image, dx1, dy1, dx1, dy2, 2, CreateRGBColor(1, 0, 0));
                DrawLine(imageReference->image, dx1, dy2, dx2, dy2, 2, CreateRGBColor(1, 0, 0));
            }
            else{
                DrawLine(imageReference->image, dx1, dy1, dx2, dy1, 2, CreateRGBColor(1, 0, 0));
                DrawLine(imageReference->image, dx2, dy1, dx2, dy2, 2, CreateRGBColor(1, 0, 0));
            }
            
        }

        std::cout << "Edges done" << std::endl;
        std::cout << "Drawing points" << std::endl;

        progress = 1;
        for(int i=0; i<n; i++){
            if(i == (n/10) * progress){
                std::cout << "Points " << (100*i / n) << "% done" << std::endl;
                progress++;
            }

            double xx = MapXCoordinateBasedOnSettings(points[i].x() - point_size/2, settings);
            double yy = MapYCoordinateBasedOnSettings(points[i].y() + point_size/2, settings);

            DrawFilledRectangle(imageReference->image, xx, yy, point_size, point_size, CreateRGBColor(0, 0, 0));
        }

        std::cout << "Points done" << std::endl;
        std::cout << "Converting to PNG" << std::endl;

        std::vector<double> *pngdata = ConvertToPNG(imageReference->image);
        WriteToFile(pngdata, file_name + ".png");
        DeleteImage(imageReference->image);

    }

    FreeAllocations();

    if(!success){
    	std::cout << "Drawing failed\n";
    	exit(EXIT_FAILURE);
    }
}
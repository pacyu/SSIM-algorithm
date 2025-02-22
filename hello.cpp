#include "stdafx.h"
#include "opencv2/opencv.hpp"
#include <Windows.h>
#include <iostream>

#pragma warning(disable:4996)

using std::cout; using std::cerr; using std::endl;
using namespace cv;

/********************************************************************************************************************************************************
* 该部分主要功能实现 SSIM 算法
*
* 关于 SSIM 算法参考文献：https://ece.uwaterloo.ca/~z70wang/publications/ssim.pdf
* 公式：$$ssim(x, y) = \frac{(2\mu _x\mu _y + C_1)(\sigma _{xy} + C_2)}{(\mu _{x}^{2} + \mu _{y}^{2} + C_1)(\sigma _{x}^{2} + \sigma _{y}^{2} + C_2)}$$
*
* 参数：
*   im1: 图像 1
*   im2: 图像 2
*   window: 滑动窗口大小，用于卷积滤波
*   k1: 可调节常数，默认 k1 = 0.01
*   k2: 可调节常数，默认 k2 = 0.03
*   L: 单通道灰度图像像素值范围，默认 L = 255.0
*
* 返回值：
*   相似度指标 (类型：double)
*********************************************************************************************************************************************************/
float ssim(Mat im1, Mat im2, int window = 7, float k1 = 0.01f, float k2 = 0.03f, float L = 255.f)
{
    CV_Assert(im1.size() == im2.size());
    
    int ndim = im1.dims;
    float NP = std::powf(window, ndim);
    float cov_norm = NP / (NP - 1);
    float C1 = (k1 * L) * (k1 * L);
    float C2 = (k2 * L) * (k2 * L);
    
    Mat ux, uy;
    Mat uxx = im1.mul(im1);
    Mat uyy = im2.mul(im2);
    Mat uxy = im1.mul(im2);
    
    blur(im1, ux, Size(window, window), Point(-1, -1));
    blur(im2, uy, Size(window, window), Point(-1, -1));
    
    blur(uxx, uxx, Size(window, window), Point(-1, -1));
    blur(uyy, uyy, Size(window, window), Point(-1, -1));
    blur(uxy, uxy, Size(window, window), Point(-1, -1));
    
    Mat ux_sq = ux.mul(ux);
    Mat uy_sq = uy.mul(uy);
    Mat uxy_m = ux.mul(uy);

    Mat vx = cov_norm * (uxx - ux_sq);
    Mat vy = cov_norm * (uyy - uy_sq);
    Mat vxy = cov_norm * (uxy - uxy_m);

    Mat A1 = 2 * uxy_m;
    Mat A2 = 2 * vxy;
    Mat B1 = ux_sq + uy_sq;
    Mat B2 = vx + vy;
    
    Mat ssim_map = (A1 + C1).mul(A2 + C2) / (B1 + C1).mul(B2 + C2);
    
    Scalar mssim = mean(ssim_map);
    ssim_map.convertTo(ssim_map, CV_8UC1, 255, 0);
    
    //imshow("im3", ssim_map);

    return mssim[0];
}

int main(int argc, const char** argv)
{
    //Mat channel;
    //split(image, channel);
    //Scalar mean, dev;
    //Mat covar;
    //meanStdDev(channel, mean, dev);
    //calcCovarMatrix(channel[0], covar, mean, COVAR_NORMAL | COVAR_USE_AVG);
    //std::cout << mean << " " << dev << " " << covar;
    //normalize(ssim_map, ssim_map, 1.0, 0.0, NORM_MINMAX);

    VideoCapture capture(0);
    
    

    Mat frame;
    Mat original_im;
    if (argc > 1)
        original_im = imread(argv[1]);
    else
        original_im = imread("C:/opencv/samples/data/lena.jpg");

    if (original_im.empty())
    {
        cerr << "ERROR: 无法打开图像." << endl;
        return 1;
    }

    imshow("original image", original_im);

    cout << "已准备好捕获屏幕 ..." << endl;

    //for (;;)
    //{
    //    HDC hdcScreen;
    //    HDC hdcWindow;
    //    HDC hdcMemDC = NULL;
    //    HBITMAP hbmScreen = NULL;
    //    BITMAP bmpScreen;

    //    Mat frame;
    //    BITMAPINFOHEADER  bi;

    //    HWND hWnd = GetDesktopWindow();

    //    hdcScreen = GetDC(NULL);
    //    hdcWindow = GetDC(hWnd);
    //    hdcMemDC = CreateCompatibleDC(hdcWindow);

    //    RECT rcClient;
    //    GetClientRect(hWnd, &rcClient);

    //    SetStretchBltMode(hdcWindow, COLORONCOLOR);
    //    StretchBlt(hdcWindow, 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, hdcWindow, 0, 0, rcClient.right, rcClient.bottom, SRCCOPY);

    //    frame.create(original_im.size().height, original_im.size().width, CV_8UC3);

    //    hbmScreen = CreateCompatibleBitmap(hdcWindow, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
    //    SelectObject(hdcMemDC, hbmScreen);
    //    BitBlt(hdcMemDC, 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, hdcWindow, 0, 0, SRCCOPY);

    //    GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

    //    bi.biSize = sizeof(BITMAPINFOHEADER); // http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
    //    bi.biWidth = bmpScreen.bmWidth;
    //    bi.biHeight = bmpScreen.bmHeight;
    //    bi.biPlanes = 1;
    //    bi.biBitCount = 32;
    //    bi.biCompression = BI_RGB;
    //    bi.biSizeImage = 0;
    //    bi.biXPelsPerMeter = 0;
    //    bi.biYPelsPerMeter = 0;
    //    bi.biClrUsed = 0;
    //    bi.biClrImportant = 0;

    //    GetDIBits(hdcWindow, hbmScreen, 0, (UINT)bmpScreen.bmHeight, frame.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    //    DeleteObject(hbmScreen);
    //    DeleteObject(hdcMemDC);
    //    ReleaseDC(NULL, hdcScreen);
    //    ReleaseDC(hWnd, hdcWindow);

    //    //resize(frame, frame, original_im.size());

    //    Mat im1f, im2f;
    //    original_im.convertTo(im1f, CV_32FC1);
    //    frame.convertTo(im2f, CV_32FC1);

    //    std::ostringstream text;
    //    text << "mssim:" << ssim(im1f, im2f);

    //    putText(frame, text.str(), Point(30, 30),
    //        HersheyFonts::FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 0, 0), 1, 8);
    //    
    //    imshow("Frame", frame);
    //    char c = (char)waitKey(20);
    //    if (c == 27 || c == 'q' || c == 'Q')
    //        break;
    //}
    
    if (capture.isOpened())
    {
        cout << "摄像头已准备好捕获 ..." << endl;

        for (;;)
        {
            capture >> frame;
            if (frame.empty())
            {
                cerr << "ERROR: 无法从摄像头捕获图像." << endl;
                break;
            }

            Mat im1f, im2f;
            original_im.convertTo(im1f, CV_32FC1);
            frame.convertTo(im2f, CV_32FC1);

            std::ostringstream text;
            text << "mssim:" << ssim(im1f, im2f);

            putText(frame, text.str(), Point(30, 30),
                FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(255, 200, 0), 1, 8);
            imshow("origin image", original_im);
            imshow("Frame", frame);

            char c = (char)waitKey(10);
            if (c == 27 || c == 'q' || c == 'Q')
                break;
        }
    }
    else {
      cerr << "ERROR: 无法打开摄像头." << endl;
      return 1;
    }

    return 0;
}

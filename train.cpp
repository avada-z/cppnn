#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <vector>
#include <bits/stdc++.h>
#include <thread>
#include <opencv2/opencv.hpp>
#include <mutex>
namespace fs = std::filesystem;
using namespace std;
string filenames[9000], filepath, modelname="test.model";
int neur[4096], pix[4096];
int pixsum=0, filenum=0;
bool READED=false;
mutex total;
void getfiles()
{
    for (const auto & entry : fs::directory_iterator(filepath))
    {
        filenum++;
    }
    int i=0;
    for (const auto & entry : fs::directory_iterator(filepath))
    {
        filenames[i]=entry.path();
        i++;
    }
}
void split(const string file)
{
    int numval=0;
    cout << file << endl;
    cv::Mat img = cv::imread(file,0);
    for(int i=0; i<img.rows; i++)
        for(int j=0; j<img.cols; j++)
        {
            if((int) img.at<uchar>(i,j)==255)
            {
                pix[numval]=1;
                numval++;
            }
            else if((int) img.at<uchar>(i,j)==254)
            {
                pix[numval]=1;
                numval++;
            }
            else
            {
                pix[numval]=0;
                numval++;
            }
        }
}
void blankmodel()
{
    ofstream model(modelname);
    for(int i=0; i<4096; i++)
    {
        model<<"0\n";
    }
    model.close();
}
void readmodel()
{
    int i=0, value;
    ifstream model(modelname);
    while (model >> value)
    {
        neur[i]=value;
        i++;
    }
    model.close();
    READED=true;
}
void summt(int start, int end)
{
    for(int i=start; i<end; i++)
    {   
        if(pix[i]==1)
        {
            total.lock();
            pixsum=pixsum+neur[i];
            total.unlock();
        }
    }
}
void pundownmt(int start, int end)
{
    for(int i=start; i<end; i++)
    {
        if(pix[i]==1)
        {
            neur[i]=neur[i]-1;
        }
    }
}
void punupmt(int start, int end)
{
    for(int i=start; i<end; i++)
    {
        if(pix[i]==1)
        {
            neur[i]=neur[i]+1;
        }
    }
}
void savemodel()
{
    ofstream modelout(modelname);
    for(int i=0; i<4096; i++)
    {
        modelout<<neur[i]<<"\n";
    }
    modelout.close();
    cout << "Model saved" << "\n";

}
void verdict(string filename)
{   
    int activator=5000;
    string jpg=".jpg", png=".png";
    cout << filename << " rating is " << pixsum << endl;
    if(pixsum>=activator && strstr(filename.c_str(),jpg.c_str()))
    {
        cout << "Running punishment down\n";
        thread pdown1(pundownmt, 0, 512);
        thread pdown2(pundownmt, 512, 1024);
        thread pdown3(pundownmt, 1024, 1536);
        thread pdown4(pundownmt, 1536, 2048);
        thread pdown5(pundownmt, 2048, 2560);
        thread pdown6(pundownmt, 2560, 3072);
        thread pdown7(pundownmt, 3072, 3584);
        thread pdown8(pundownmt, 3584, 4096);
        pdown1.join();
        pdown2.join();
        pdown3.join();
        pdown4.join();
        pdown5.join();
        pdown6.join();
        pdown7.join();
        pdown8.join();
    }
    if(pixsum<activator && strstr(filename.c_str(),png.c_str()))
    {
        cout << "Running punishment up\n";
        thread pup1(punupmt, 0, 512);
        thread pup2(punupmt, 512, 1024);
        thread pup3(punupmt, 1024, 1536);
        thread pup4(punupmt, 1536, 2048);
        thread pup5(punupmt, 2048, 2560);
        thread pup6(punupmt, 2560, 3072);
        thread pup7(punupmt, 3072, 3584);
        thread pup8(punupmt, 3584, 4096);
        pup1.join();
        pup2.join();
        pup3.join();
        pup4.join();
        pup5.join();
        pup6.join();
        pup7.join();
        pup8.join();
    }
}
void train()
{
    int cycles=0, fith=0;
    cout <<"How many cycles perform?: ";
    cin >> cycles;
    cout << "Saving model every 512 images\n";
    getfiles();
    for (int j=0; j< cycles; j++)
    {
        for(int i=0; i<filenum; i++)
        {
            split(filenames[i]);
            if(READED==false)
            {
                readmodel();
            }
            if(fith==512)
            {
                savemodel();
                fith=0;
            }
            fith++;
            pixsum=0;
            thread thread1(summt, 0, 512);
            thread thread2(summt, 512, 1024);
            thread thread3(summt, 1024, 1536);
            thread thread4(summt, 1536, 2048);
            thread thread5(summt, 2048, 2560);
            thread thread6(summt, 2560, 3072);
            thread thread7(summt, 3072, 3584);
            thread thread8(summt, 3584, 4096);
            thread1.join();
            thread2.join();
            thread3.join();
            thread4.join();
            thread5.join();
            thread6.join();
            thread7.join();
            thread8.join();
            cout << "Processing image " << filenames[i] << " " << fith << "/512 before saving...\n";
            verdict(filenames[i]);
        }
    }
}
int main()
{
    char mod;
    cout << "Training dir path: ";
    cin>>filepath;
    cout << "Create new model? (Y/N): ";
    cin >> mod;
    if(mod=='Y')
    {
        cout << "Enter new model name: ";
        cin >> modelname;
        blankmodel();
    }
    else
    {
        cout << "Enter model name: ";
        cin >> modelname;
    }
    train();
    return 0;
}
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <regex>
#include <vector>
#include <bits/stdc++.h>
#include <thread>
namespace fs = std::filesystem;
using namespace std;
int neur[4095];
int pix[4095];
string filenames[90000];
int pixsum=0;
int filenum=0;
string path;
bool READED=false;
string modelname="test.model";
thread threads[63] = {};
void getfiles()
{
    for (const auto & entry : fs::directory_iterator(path))
    {
        filenum++;
    }
    int i=0;
    for (const auto & entry : fs::directory_iterator(path))
    {
        filenames[i]=entry.path();
        i++;
    }
}
void split(string file)
{
    string line;
    string foo;
    string bar;
    string threshold="convert ";
    threshold+=file;
    threshold+=" -threshold 30% ";
    threshold+=file;
    system(threshold.c_str());
    threshold="convert ";
    threshold+=file;
    threshold+=" sparse-color: > img_temp.txt";
    system(threshold.c_str());
    ifstream image("img_temp.txt");
    ofstream out("img_temp2.txt");
    while(image >> line)
    {
        line=regex_replace(line, regex(" "), "\n");
        line=regex_replace(line, regex(","), " ");
        out<<line<<endl; 
    }
    out.close();
    ifstream image2("img_temp2.txt");
    ofstream out2("current_image_temp.txt");
    while(image2 >> foo >> bar >> line)
    {   
        line=regex_replace(line, regex("gray[(]255[)]"), "1");
        line=regex_replace(line, regex("gray[(]0[)]"), "0");
        out2<<line<<endl;
    }
    out2.close();
}
void blankmodel()
{
    ofstream model(modelname);
    for(int i=0; i<4096; i++)
    {
        model<<"0"<<endl;
    }
    model.close();
}
void readmodel()
{
    int i=0;
    int value;
    ifstream model(modelname);
    while (model >> value)
    {
        neur[i]=value;
        i++;
    }
    READED=true;
}
void readpix()
{
    int pixvalue;
    int i=0;
    ifstream pixels("current_image_temp.txt");
    while (pixels >> pixvalue)
    {
        pix[i]=pixvalue;
        i++;
    }
}
void summt(int start, int end)
{
    for(int i=start; i<end; i++)
    {   
        if(pix[i]==1)
        {
            pixsum=pixsum+neur[i];
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
    ofstream model(modelname);
    for(int i=0; i<4096; i++)
    {
        model<<neur[i]<<endl;
        cout << "Saving model..." << endl;
    }
    model.close();
}
void verdict(string filename)
{   
    int activator=5000;
    int j=0;
    string jpg=".jpg";
    string png=".png";
    cout << filename << " rating is " << pixsum << endl;
    if(pixsum>=activator && strstr(filename.c_str(),jpg.c_str()))
    {
        cout << "running punishment down" << endl;
        for(int i=0; i+64<=4096; i+=64)
        {
        threads[j] = thread(pundownmt, i, i+64);
        threads[j].join();
        j++;
        }
    }
    if(pixsum<activator && strstr(filename.c_str(),png.c_str()))
    {
        cout << "running punishment up" << endl;
        for(int i=0; i+64<=4096; i+=64)
        {
        threads[j] = thread(punupmt, i, i+64);
        threads[j].join();
        j++;
        }
    }
}
void train()
{
    int cycles=0;
    int fith=0;
    int j=0;
    cout <<"How many cycles perform?: ";
    cin >> cycles;
    cout << "Saving model every 500 images" << endl;
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
            if(fith==500)
            {
                savemodel();
                fith=0;
            }
            fith++;
            readpix();
            pixsum=0;
            j=0;
            for(int i=0; i+64<=4096; i+=64)
            {
                threads[j] = thread(summt, i, i+64);
                threads[j].join();
                j++;
            }
            cout << "Processing image " << fith << "/500 before saving..." << endl;
            verdict(filenames[i]);
        }
    }
}
int main()
{
    char mod;
    cout << "Training dir path: ";
    cin>>path;
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
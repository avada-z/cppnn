#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <vector>
#include <bits/stdc++.h>
#include <thread>
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
    int pixnum=0;
    string line, foo, bar, threshold="convert ";
    threshold=threshold+file+" -threshold 30% " + file;
    system(threshold.c_str());
    threshold="convert " + file +" sparse-color: > img_temp.txt";
    system(threshold.c_str());
    ifstream image("img_temp.txt");
    ofstream out("img_temp2.txt");
    while(image >> line)
    {
        for (int i = 0; i < line.size(); i++)
        {
            if (line[i] == ' ') 
            {
                line[i] = '\n';
            }
            if (line[i] == ',')
            {
                line[i] = ' ';
            }
        }
        out<<line<<endl; 
    }
    out.close();
    ifstream image2("img_temp2.txt");
    while(image2 >> line)
    {   
        stringstream X(line);
        while (getline(X, foo, ' '))
        {  
            if(foo=="gray(255)")
            {
                pix[pixnum] = 1;
                pixnum++;
            }
            else if(foo=="gray(254)")
            {
                pix[pixnum] = 1;
                pixnum++;
            }
            else if(foo=="gray(0)")
            {
                pix[pixnum] = 0;
                pixnum++;
            }
        }  
    }
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
        modelout<<neur[i]<<endl;
    }
    modelout.close();
    cout << "Model saved" << endl;

}
void verdict(string filename)
{   
    int activator=5000;
    string jpg=".jpg", png=".png";
    cout << filename << " rating is " << pixsum << endl;
    if(pixsum>=activator && strstr(filename.c_str(),jpg.c_str()))
    {
        cout << "Running punishment down" << endl;
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
        cout << "Running punishment up" << endl;
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
            cout << "Processing image " << filenames[i] << " " << fith << "/500 before saving..." << endl;
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